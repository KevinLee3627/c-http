#include <sys/types.h>
#include <stdio.h>
int parse_request(char *req_buffer, ssize_t req_length)
{
  req_buffer[req_length] = '\0';
  char *start_line;
  for (int i = 0; i < req_length; i++)
  {
    if (req_buffer[i] == '\n')
    {
      // printf("^");
    }
    printf("%c", req_buffer[i]);
  }
  return 1;
}