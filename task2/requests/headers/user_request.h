#ifndef USER_REQUEST_H
#define USER_REQUEST_H
#define USERNAME_LEN 128

#include "connection_request.h"

struct user_request {
  char username[USERNAME_LEN];
  enum connection_type type;
};

#endif // !USER_REQUEST_H
