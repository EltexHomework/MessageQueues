#ifndef CHAT_H
#define CHAT_H

#include "chat_window.h"
#include "input_field.h"
#include "users_window.h"
#include "../../server/headers/config.h"
#include "../../requests/headers/users_msg.h"
#include "../../requests/headers/message_msg.h"
#include "../../requests/headers/connection_msg.h"
#include <sys/msg.h>
#include <unistd.h>
#include <pthread.h>

struct chat {
  struct chat_window* chat_window;
  struct users_window* users_window;
  struct input_field* input_field;
  char username[USERNAME_LEN];

  int login_queue;
  int messages_queue;
  int users_queue;
  int new_messages_queue;
  
  // required for direct receive of messages
  long pid;

  // 1 - chat, 2 - users, 3 - input_field
  int current_window;  

};

struct chat* create_chat(char* username, int height, int width, int start_y, int start_x);

void run_chat(struct chat* chat);

void connect_to_server(struct chat* chat);

void disconnect_from_server(struct chat* chat);

void* handle_user_requests(void* args);

void* handle_message_requests(void* args);

void send_message(struct chat* chat);

int open_queue(char* filename, int id);
  
void dispose_chat(struct chat* chat);

#endif // !CHAT_H
