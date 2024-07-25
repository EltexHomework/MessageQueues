#ifndef USER_H
#define USER_H
#define USERNAME_LEN 128

#include <sys/types.h>
#include <unistd.h>

struct user {
  char username[USERNAME_LEN];
  long pid; 
};

#endif // !USER_H
