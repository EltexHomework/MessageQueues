#ifndef USER_MSG_H
#define USER_MSG_H

#include "user_request.h"

/*
 * User msg struct implements message for message
 * queues. Used to notify clients about new users 
 * or disconnected users.
 * To send or receive this struct you should take
 * sizeof of payload.
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

struct user_msg {
  /* pid of receiver */
  long mtype;
  /* payload of message */
  struct user_request request;
};

#endif // !USER_MSG_H
