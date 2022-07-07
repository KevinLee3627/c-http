#ifndef ERRORS_H
#define ERRORS_H

#include "protocol.h"

enum ERROR_CODES
{
  MALFORMED_STATUS_LINE = 1,
  JUST_FOR_FUN
};

const char *const ERROR_MESSAGES[] = {
    [MALFORMED_STATUS_LINE] = "Status line is malformed",
    [JUST_FOR_FUN] = "Testing"};
#endif