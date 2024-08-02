#ifndef MESSAGE_REQUEST_H
#define MESSAGE_REQUEST_H

#include "../../server/headers/config.h"

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
struct message_request {
  char username[USERNAME_LEN];
  char message[MESSAGE_LEN];
};

#endif // MESSAGE_REQUEST_H
