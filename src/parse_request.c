#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "protocol.h"
#include "errors.h"

int parse_request(char *req_buffer, ssize_t req_length, struct HTTPRequest *http_request)
{
  // Status line : METHOD PATH HTTP - VERSION\r\n
  req_buffer[req_length] = '\0';

  // char *status_line_terminator = strstr(req_buffer, "\r\n");
  char *status_line_start = &req_buffer[0];

  // Find first space occurence - start to first space occurence = the method
  char *start = status_line_start;
  char *first_space = strstr(start, " ");                   // pointer to the first space
  size_t method_length = (size_t)(first_space - start + 1); // +1 for null terminator

  http_request->method = malloc(method_length);
  memcpy(http_request->method, start, method_length - 1);
  http_request->method[method_length - 1] = '\0';

  // first_space is a pointer pointing to the actual space - to find the 2nd space,
  // you need to start searching from first_space + 1
  char *second_space = strstr(first_space + 1, " ");
  if (second_space == NULL)
  {
    return MALFORMED_STATUS_LINE;
  }
  size_t path_length = (size_t)(second_space - first_space);
  http_request->path = malloc(path_length);
  // -1 b/c path_length includes space for null terminator. -1 makes sure we only get the actual chars
  memcpy(http_request->path, first_space + 1, path_length - 1);
  http_request->path[path_length - 1] = '\0';

  return 1;
}
