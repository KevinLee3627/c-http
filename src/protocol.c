#include <stdlib.h>
#include <stdio.h>

#include "dictionary.h"
#include "protocol.h"

struct HTTPRequest *init_http_request(void)
{
  struct HTTPRequest *http_request = malloc(sizeof(struct HTTPRequest));
  if (http_request == NULL)
    return NULL;

  http_request->method = NULL;
  http_request->path = NULL;
  http_request->version = NULL;
  http_request->headers = init_hash_table(BUCKET_COUNT);
  http_request->body = NULL;
  return http_request;
}

void free_http_request(struct HTTPRequest *http_request)
{
  free(http_request->method);
  free(http_request->path);
  free(http_request->version);
  free_table(http_request->headers);
  free(http_request->body);
}