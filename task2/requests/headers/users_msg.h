#ifndef USER_MSG_H
#define USER_MSG_H

#include "user_request.h"

struct user_msg {
  long mtype;
  struct user_request request;
};

#endif // !USER_MSG_H
