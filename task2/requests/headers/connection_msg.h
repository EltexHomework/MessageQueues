#ifndef CONNECTION_MSG_H
#define CONNECTION_MSG_H

#include "connection_request.h"

struct connection_msg {
  long mtype;
  struct connection_request request;
};

#endif // CONNECTION_MSG_H
