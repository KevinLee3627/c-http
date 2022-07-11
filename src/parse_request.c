#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "protocol.h"
#include "tokenizer.h"
#include "dictionary.h"

int parse_request(char *req_buffer, struct HTTPRequest *http_request)
{
  struct Tokenizer *request_tokenizer = init_tokenizer(req_buffer);
  get_token(request_tokenizer, " ");
  http_request->method = request_tokenizer->token;
  request_tokenizer->buffer = request_tokenizer->remaining_buffer;

  get_token(request_tokenizer, " ");
  http_request->path = request_tokenizer->token;
  request_tokenizer->buffer = request_tokenizer->remaining_buffer;

  get_token(request_tokenizer, "\r\n");
  http_request->version = request_tokenizer->token;
  request_tokenizer->buffer = request_tokenizer->remaining_buffer;

  while (get_token(request_tokenizer, "\r\n") == 0)
  {
    printf("Token: %s\n", request_tokenizer->token);
    if (strcmp(request_tokenizer->token, "") == 0)
    {
      printf("end of headers\n");
      free(request_tokenizer->token);
      break;
    }
    free(request_tokenizer->token);
    request_tokenizer->buffer = request_tokenizer->remaining_buffer;
  }

  free(request_tokenizer);
  return 0;
}
