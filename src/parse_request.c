#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "protocol.h"

int parse_request(char *req_buffer, ssize_t req_length, struct HTTPRequest *http_request)
{
  // TODO: Error handling?
  char *cursor;
  printf("Splitting req_buffer\n");
  cursor = strtok(req_buffer, " ");
  http_request->method = cursor;

  cursor = strtok(NULL, " ");
  http_request->path = cursor;

  cursor = strtok(NULL, "\n");
  http_request->version = cursor;

  return 0;
}
