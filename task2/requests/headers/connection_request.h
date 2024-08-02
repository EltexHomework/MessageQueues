#ifndef CONNECTION_REQUEST_H
#define CONNECTION_REQUEST_H

#include "../../server/headers/config.h"

enum connection_type { CONNECT, DISCONNECT };
/*
 * Connection request struct implements a payload for connection_msg.
 * Used to specify connection action of user with username.
 * [Example]
 * int qid;
 * struct connection_msg message;
 * 
 * if (msgsnd(qid, &message, sizeof(message.request), 0) != -1) {
 *  perror("msgsnd error");
 *  exit(EXIT_FAILURE);
 * }
 * 
 * if (msgrcv(qid, &message, sizeof(message.request), 0) != -1) {
 *  perror("msgsnd error");
 *  exit(EXIT_FAILURE);
 * }
 */
struct connection_request {
  char username[USERNAME_LEN];
  enum connection_type type;   
};

#endif // !CONNECTION_REQUEST_H
