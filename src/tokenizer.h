#ifndef TOKENIZER_H
#define TOKENIZER_H

struct Tokenizer
{
  char *buffer;    // String to tokenize
  char *token;     // Token returned from get_token
  char *token_end; // Pointer to end of token, returned from get_token
};

struct Tokenizer *init_tokenizer(char *str);
int get_token(struct Tokenizer *tokenizer, const char *delimiter);

#endif