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
    char *header_line = request_tokenizer->token;
    if (strcmp(header_line, "") == 0)
    {
      printf("end of headers\n");
      free(header_line);
      break;
    }
    struct Tokenizer *header_tokenizer = init_tokenizer(header_line);
    get_token(header_tokenizer, ": ");

    size_t header_key_len = strlen(header_tokenizer->token);
    char header_key[header_key_len + 1];
    strncpy(header_key, header_tokenizer->token, header_key_len);
    header_key[header_key_len] = '\0';

    size_t header_val_len = strlen(header_tokenizer->remaining_buffer);
    char header_val[header_val_len + 1];
    strncpy(header_val, header_tokenizer->remaining_buffer, header_val_len);
    header_val[header_val_len] = '\0';

    insert(http_request->headers, header_key, header_val);

    free(header_tokenizer->token);
    free(header_tokenizer);
    free(header_line);
    request_tokenizer->buffer = request_tokenizer->remaining_buffer;
  }
  free(request_tokenizer);
  return 0;
}
