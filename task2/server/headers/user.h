#ifndef USER_H
#define USER_H

#include "config.h"
#include <sys/types.h>
#include <unistd.h>

struct user {
  char username[USERNAME_LEN];
  long pid; 
};

#endif // !USER_H
