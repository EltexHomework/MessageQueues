#ifndef MESSAGE_H
#define MESSAGE_H


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../headers/config.h"

/*
 * Message struct specifies user message,
 * contains username length of USERNAME_LEN,
 * text length of MESSAGE_LEN. Lengths are
 * specified in config.h file.
 */
struct message {
  char username[USERNAME_LEN];
  char text[MESSAGE_LEN];
};

#endif // !MESSAGE_H
