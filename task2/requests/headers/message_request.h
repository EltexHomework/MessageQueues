#ifndef MESSAGE_REQUEST_H
#define MESSAGE_REQUEST_H

#include "../../server/headers/config.h"

struct message_request {
  char username[USERNAME_LEN];
  char message[MESSAGE_LEN];
};

#endif // MESSAGE_REQUEST_H
