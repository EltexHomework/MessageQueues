#ifndef MESSAGE_MSG_H 
#define MESSAGE_MSG_H 

#include "message_request.h"

struct message_msg {
  long mtype;
  struct message_request request;
};

#endif // MESSAGE_MSG_H 
