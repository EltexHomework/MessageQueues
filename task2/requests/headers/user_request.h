#ifndef USER_REQUEST_H
#define USER_REQUEST_H

#include "connection_request.h"

/*
 * User request struct implements a payload for user_msg.
 * Specifies connection action of user with username.
 * [Example]
 * int qid;
 * struct user_msg message;
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
struct user_request {
  char username[USERNAME_LEN];
  enum connection_type type;
};

#endif // !USER_REQUEST_H
