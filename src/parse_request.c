#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "protocol.h"
#include "tokenizer.h"

int parse_request(char *req_buffer, struct HTTPRequest *http_request)
{
  struct Tokenizer *request_tokenizer = init_tokenizer(req_buffer);
  get_token(request_tokenizer, " ");
  http_request->method = request_tokenizer->token;
  request_tokenizer->buffer = request_tokenizer->remaining_buffer;

  get_token(request_tokenizer, " ");
  http_request->path = request_tokenizer->token;
  request_tokenizer->buffer = request_tokenizer->remaining_buffer;

  int status = get_token(request_tokenizer, "\r\n");
  http_request->version = request_tokenizer->token;

  free(request_tokenizer);
  return 0;
}
