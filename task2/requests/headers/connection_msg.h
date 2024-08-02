#ifndef CONNECTION_MSG_H
#define CONNECTION_MSG_H

#include "connection_request.h"
/*
 * Connection message struct implements a message
 * for message queues. Used as a buffer for msgsnd
 * and msgrcv. 
 * To send or receive this struct you should take
 * sizeof of payload.
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
struct connection_msg {
  /* pid of receiver */
  long mtype;
  /* payload of message */ 
  struct connection_request request;
};

#endif // CONNECTION_MSG_H
