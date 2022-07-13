#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <netinet/in.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "parse_request.h"
#include "send_response.h"
#include "protocol.h"
#include "dictionary.h"

#define BACKLOG_COUNT 20
#define MAX_FORKS 5

int max_forks = MAX_FORKS;
int current_forks = 0;

int create_socket(char *port)
{
  struct addrinfo hints;
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE; // fill in my own IP

  struct addrinfo *gai_res = NULL; // getaddrinfo_response
  int gai_status = getaddrinfo(NULL, port, &hints, &gai_res);
  if (gai_status != 0)
  {
    perror("getaddrinfo");
    return 1;
  }

  // socket() creates a socket and returns the socket file descriptor
  int listening_socket_fd =
      socket(gai_res->ai_family, gai_res->ai_socktype, gai_res->ai_protocol);
  if (listening_socket_fd == -1)
  {
    freeaddrinfo(gai_res);
    perror("socket");
    return 1;
  }

  // Allows the reuse of this port, avoiding 'address already in use' error in bind()
  int yes = 1;
  if (setsockopt(listening_socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == -1)
  {
    perror("error w/ setsockopt: ");
    exit(1);
  }

  if (bind(listening_socket_fd, gai_res->ai_addr, gai_res->ai_addrlen) == -1)
  {
    close(listening_socket_fd);
    freeaddrinfo(gai_res);
    perror("bind");
    return 1;
  }

  freeaddrinfo(gai_res);

  if (listen(listening_socket_fd, BACKLOG_COUNT) == -1)
  {
    perror("listen");
    return 1;
  }

  return listening_socket_fd;
}

SSL_CTX *create_context(void)
{
  const SSL_METHOD *method;
  SSL_CTX *ctx;

  method = TLS_server_method();
  ctx = SSL_CTX_new(method);
  if (ctx == NULL)
  {
    perror("Unable to create SSL context");
    ERR_print_errors_fp(stderr);
    exit(EXIT_FAILURE);
  }

  return ctx;
}

void configure_server_context(SSL_CTX *ctx)
{
  if (SSL_CTX_use_certificate_file(ctx, "./ssl/public.pem", SSL_FILETYPE_PEM) <= 0)
  {
    ERR_print_errors_fp(stderr);
    exit(EXIT_FAILURE);
  }

  if (SSL_CTX_use_PrivateKey_file(ctx, "./ssl/key.pem", SSL_FILETYPE_PEM) <= 0)
  {
    ERR_print_errors_fp(stderr);
    exit(EXIT_FAILURE);
  }
}

void ssl_cleanup(SSL *ssl)
{
  SSL_shutdown(ssl);
  SSL_free(ssl);
}

int main(int argc, char **argv)
{
  if (argc != 2)
  {
    printf("usage: ./server PORT\n");
    return 1;
  }

  char *port = argv[1];
  // Port cannot have a leading zero
  if (*port == '0')
  {
    printf("Invalid PORT: No leading zeros\n");
    return 1;
  }
  for (char *c = port; *c != '\0'; c++)
  {
    if (isdigit(*c) == 0)
    {
      printf("Invalid PORT: Must only contain digits\n");
      return 1;
    }
  }

  // Create SSL context
  SSL_CTX *ssl_ctx = NULL;
  ssl_ctx = create_context();
  configure_server_context(ssl_ctx);

  int listening_socket_fd = create_socket(port);

  int incoming_socket;

  struct sockaddr_storage incoming_addr;
  socklen_t incoming_addr_size = sizeof incoming_addr;
  incoming_socket =
      accept(listening_socket_fd, (struct sockaddr *)&incoming_addr,
             &incoming_addr_size);
  if (incoming_socket == -1)
  {
    perror("error w/ accept:");
    return 1;
  }

  char ipstr[INET6_ADDRSTRLEN + 1];
  inet_ntop(incoming_addr.ss_family, (struct sockaddr *)&incoming_addr, ipstr, sizeof(char) * (INET6_ADDRSTRLEN) + 1);
  printf("TCP Connection from %s\n", ipstr);

  SSL *ssl = NULL;
  ssl = SSL_new(ssl_ctx);
  SSL_set_fd(ssl, incoming_socket);

  if (SSL_accept(ssl) <= 0)
  {
    ERR_print_errors_fp(stderr);
  }

  printf("SSL Connection accepted\n");

  pid_t child_pid = fork();
  if (child_pid < 0)
  {
    close(incoming_socket);
  }
  else if (child_pid == 0)
  {
    // This is the child process
    const size_t req_buffer_length = sizeof(char) * 2000;
    char *request_buffer = malloc(req_buffer_length);
    // ssize_t bytes_received = recv(incoming_socket, request_buffer, req_buffer_length - 1, 0);
    int bytes_received = SSL_read(ssl, request_buffer, (int)req_buffer_length);
    if (bytes_received < 0)
    {
      free(request_buffer);
      ssl_cleanup(ssl);
      ERR_print_errors_fp(stderr);
      exit(1);
    }
    if (bytes_received == 0)
    {
      free(request_buffer);
      printf("Connection closed by peer.\n");
      ssl_cleanup(ssl);
      close(incoming_socket);
      exit(0);
    }
    request_buffer[req_buffer_length - 1] = '\0';

    struct HTTPRequest *http_request = init_http_request();
    int parse_req_status = parse_request(request_buffer, http_request);
    if (parse_req_status > 0)
    {
      exit(1);
    }
    send_response(ssl, http_request->path);

    free_http_request(http_request);
    ssl_cleanup(ssl);

    close(incoming_socket);
    exit(0); // Exits the forked child process
  }
  else
  {
    // This is the parent process, no need to care about incoming socket
    close(incoming_socket);
    current_forks++;
  }

  // Delete finished sub processes if (current_forks >= max_forks)
  if (current_forks >= max_forks)
  {
    while (waitpid(-1, NULL, WNOHANG) > 0)
    {
      current_forks--;
    }
  }
  close(listening_socket_fd);

  return 0;
}
