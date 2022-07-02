#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

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

  int bind_status =
      bind(listening_socket_fd, gai_res->ai_addr, gai_res->ai_addrlen);
  if (bind_status == -1)
  {
    freeaddrinfo(gai_res);
    perror("error w/ bind:");
    return 1;
  }

  int listen_status = listen(listening_socket_fd, BACKLOG_COUNT);
  if (listen_status == -1)
  {
    freeaddrinfo(gai_res);
    perror("error w/ listen:");
    return 1;
  }

  struct sockaddr_storage incoming_addr;
  socklen_t incoming_addr_size = sizeof incoming_addr;
  int incoming_fd =
      accept(listening_socket_fd, (struct sockaddr *)&incoming_addr,
             &incoming_addr_size);
  if (incoming_fd == -1)
  {
    freeaddrinfo(gai_res);
    perror("error w/ accept:");
    return 1;
  }
  freeaddrinfo(gai_res);

  return 0;
}