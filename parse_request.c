#include <sys/types.h>
#include <stdio.h>
int parse_request(char *req_buffer, ssize_t req_length)
{
  req_buffer[req_length] = '\0';
  printf("%s\n", req_buffer);
  return 1;
}