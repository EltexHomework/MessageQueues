#ifndef MESSAGE_H
#define MESSAGE_H


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../headers/config.h"

struct message {
  char username[USERNAME_LEN];
  char text[MESSAGE_LEN];
};

#endif // !MESSAGE_H
