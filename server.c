#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MY_PORT "8443"
#define BACKLOG_COUNT 20

int main(void)
{
  struct addrinfo hints;
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE; // fill in my own IP

  struct addrinfo *gai_res = NULL; // getaddrinfo_response
  getaddrinfo(NULL, MY_PORT, &hints, &gai_res);

  // socket() creates a socket and returns the socket file descriptor
  int listening_socket_fd =
      socket(gai_res->ai_family, gai_res->ai_socktype, gai_res->ai_protocol);
  if (listening_socket_fd == -1)
  {
    freeaddrinfo(gai_res);
    perror("error w/ socket");
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
    perror("error w/ bind:");
    return 1;
  }

  freeaddrinfo(gai_res);

  int listen_status = listen(listening_socket_fd, BACKLOG_COUNT);
  if (listen_status == -1)
  {
    perror("error w/ listen:");
    return 1;
  }

  int incoming_fd;
  while (1)
  {
    struct sockaddr_storage incoming_addr;
    socklen_t incoming_addr_size = sizeof incoming_addr;
    incoming_fd =
        accept(listening_socket_fd, (struct sockaddr *)&incoming_addr,
               &incoming_addr_size);
    if (incoming_fd == -1)
    {
      perror("error w/ accept:");
      return 1;
    }

    char ipstr[INET6_ADDRSTRLEN + 1];
    inet_ntop(incoming_addr.ss_family, (struct sockaddr *)&incoming_addr, ipstr, sizeof(char) * (INET6_ADDRSTRLEN) + 1);
    printf("serve: Connection received from %s\n", ipstr);

    if (!fork())
    {
      close(listening_socket_fd);
      printf("Closed listening socket\n");
      char *msg = "what's up?\n";
      ssize_t bytes_sent = send(incoming_fd, msg, strlen(msg), 0);
      if (bytes_sent == -1)
      {
        perror("error w/ send:");
        return 1;
      }

      const size_t request_length = sizeof(char) * 4000;
      char *request_buffer = malloc(request_length);
      if (recv(incoming_fd, request_buffer, request_length, 0) == -1)
      {
        perror("error w/ recv:");
        return 1;
      }
      printf("%s\n", request_buffer);
      free(request_buffer);
      close(incoming_fd);
      printf("closed incoming socket\n");
      exit(0);
    }
    close(incoming_fd);
  }

  return 0;
}