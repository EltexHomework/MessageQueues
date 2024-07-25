#ifndef MESSAGE_REQUEST_H
#define MESSAGE_REQUEST_H

#define MESSAGE_LEN 128
#define USERNAME_LEN 128

struct message_request {
  char username[USERNAME_LEN];
  char message[MESSAGE_LEN];
};

#endif // MESSAGE_REQUEST_H
