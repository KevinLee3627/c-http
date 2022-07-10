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
#endif