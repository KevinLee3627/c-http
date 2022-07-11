#ifndef PROTOCL_H
#define PROTOCL_H
struct HTTPRequest
{
  char *method;
  char *path;
  char *version;
  struct HashTable *headers;
  char *body;
};

struct Header
{
  char *name;
  char *value;
};

struct HTTPRequest *init_http_request(void);
void free_http_request(struct HTTPRequest *http_request);

#endif