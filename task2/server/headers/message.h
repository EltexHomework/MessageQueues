#ifndef MESSAGE_H
#define MESSAGE_H

#define MESSAGE_LEN 128
#define USERNAME_LEN 128

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

struct message {
  char username[USERNAME_LEN];
  char text[MESSAGE_LEN];
};

char* toString(const struct message message);

#endif // !MESSAGE_H
