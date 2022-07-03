#ifndef PARSE_REQUEST_H
#define PARSE_REQUEST_H

enum METHOD
{
  GET,
  POST
};

struct req_details
{
  enum METHOD method;
  char *path;
};

int parse_request(char *req_buffer, ssize_t req_length, char **method);
#endif