#ifndef PARSE_REQUEST_H
#define PARSE_REQUEST_H

#include "protocol.h"

enum METHOD
{
  GET,
};

int parse_request(char *req_buffer, struct HTTPRequest *http_request);
#endif