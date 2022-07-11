#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "tokenizer.h"

struct Tokenizer *init_tokenizer(char *str)
{
  struct Tokenizer *tokenizer = malloc(sizeof(struct Tokenizer));
  tokenizer->buffer = str;
  tokenizer->token = NULL;
  tokenizer->token_end = NULL;
  return tokenizer;
}

int get_token(struct Tokenizer *tokenizer, const char *delimiter)
{
  size_t search_len = strlen(delimiter);
  char *start_ptr = tokenizer->buffer;
  char *end_ptr = strstr(tokenizer->buffer, delimiter);
  if (end_ptr == NULL)
    return -1;

  size_t token_len = (uintptr_t)end_ptr - (uintptr_t)start_ptr;

  tokenizer->token = malloc(token_len + 1);
  if (tokenizer->token == NULL)
  {
    free(tokenizer->token);
    return -2;
  }
  // Copy string + null terminator into tokenizer
  strncpy(tokenizer->token, start_ptr, token_len);
  tokenizer->token[token_len] = '\0';

  // Return the location of delimiter to tokenizer
  // so user can search through rest of buffer
  tokenizer->token_end = end_ptr + search_len;
  return 0;
}