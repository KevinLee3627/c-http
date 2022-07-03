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

#include "parse_request.h"
#include "send_response.h"

#define MY_PORT "8443"
#define BACKLOG_COUNT 20
#define MAX_FORKS 5

int max_forks = MAX_FORKS;
int current_forks = 0;
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

  struct sockaddr_storage incoming_addr;
  socklen_t incoming_addr_size = sizeof incoming_addr;

  while (1)
  {
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

    pid_t child_pid = fork();
    if (child_pid < 0)
    {
      close(incoming_fd);
    }
    else if (child_pid == 0)
    {
      // This is the child process
      const size_t req_buffer_length = sizeof(char) * 800;
      char *request_buffer = malloc(req_buffer_length);
      ssize_t bytes_received = recv(incoming_fd, request_buffer, req_buffer_length - 1, 0);

      if (bytes_received == -1)
      {
        free(request_buffer);
        perror("error w/ recv:");
        return 1;
      }

      if (bytes_received == 0)
      {
        free(request_buffer);
        printf("Connection closed by peer.\n");
        close(incoming_fd);
        exit(0);
      }
      parse_request(request_buffer, bytes_received);
      // send_response(incoming_fd);
      free(request_buffer);

      close(incoming_fd);
      exit(0); // Exits the forked child process
    }
    else
    {
      // This is the parent process, no need to care about incoming socket
      close(incoming_fd);
      current_forks++;
    }

    // Delete finished sub processes
    if (current_forks >= max_forks)
    {
      while (waitpid(-1, NULL, WNOHANG) > 0)
      {
        current_forks--;
      }
    }
  }
  close(listening_socket_fd);

  return 0;
}
