#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int parse_request(char *req_buffer, ssize_t req_length, char **method, char **path)
{
  // Status line: METHOD PATH HTTP-VERSION\r\n
  req_buffer[req_length] = '\0';
  for (int i = 0; i < req_length; i++)
  {
    printf("%c", req_buffer[i]);
  }
  // Find first space occurence - start to first space occurence = the method
  char *start = &req_buffer[0];
  char *first_space = strstr(start, " ");         // pointer to the first space
  size_t method_length = first_space - start + 1; // +1 for null terminator
  printf("method length: %li\n", method_length);

  // Allocate memory, copy the method, then add a null terminator
  *method = malloc(method_length);
  memcpy(*method, start, method_length - 1);
  (*method)[method_length - 1] = '\0';

  // first_space is a pointer pointing to the actual space - to find the 2nd space,
  // you need to start searching from first_space + 1
  char *second_space = strstr(first_space + 1, " ");
  if (second_space == NULL)
  {
    printf("Invalid request - no HTTP version");
    return -1;
  }
  size_t path_length = second_space - first_space;
  printf("path_length: %li\n", path_length);

  *path = malloc(path_length);
  // -1 b/c path_length includes space for null terminator. -1 makes sure we only get the actual chars
  memcpy(*path, first_space + 1, path_length - 1);
  (*path)[path_length - 1] = '\0';

  return 1;
}
