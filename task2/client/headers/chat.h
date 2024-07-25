#ifndef CHAT_H
#define CHAT_H

#include "chat_window.h"
#include "input_field.h"
#include "users_window.h"

struct chat {
  struct chat_window* chat_window;
  struct users_window* users_window;
  struct input_field* input_field;
  
  // 1 - chat, 2 - users, 3 - input_field
  int current_window;  
};

struct chat* create_chat();

void run_chat(struct chat* chat);

void dispose_chat(struct chat* chat);

#endif // !CHAT_H
