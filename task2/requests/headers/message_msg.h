#ifndef MESSAGE_MSG_H 
#define MESSAGE_MSG_H 

#include "message_request.h"

/*
 * Message request struct implements a payload for message_msg.
 * Used to specify message that user with username sent.
 * [Example]
 * int qid;
 * struct message_msg message;
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
struct message_msg {
  /* pid of receiver */
  long mtype;
  /* payload of message */
  struct message_request request;
};

#endif // MESSAGE_MSG_H 
