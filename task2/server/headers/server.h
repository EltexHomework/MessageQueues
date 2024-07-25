#ifndef SERVER_H
#define SERVER_H

#define MAX_CHAT_MESSAGES 128
#define MAX_USERS 32
#define SERVER_QUEUE "/server"
#define LOGIN_QUEUE_ID 1
#define UPDATE_MESSAGES_QUEUE_ID 2 
#define UPDATE_USERS_QUEUE_ID 3      
#define NEW_MESSAGES_QUEUE_ID 4      

#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <pthread.h>
#include "../../requests/headers/users_msg.h"
#include "../../requests/headers/connection_msg.h"
#include "../../requests/headers/message_msg.h"
#include "user.h"
#include "message.h"

struct server {
  struct user** users;
  struct message** messages;
  int users_size;
  int messages_size;

  int login_queue;
  int messages_queue;
  int users_queue;
  int new_messages_queue;
};

struct server* create_server();

void run_server(struct server* server);

void* handle_connection_requests(void* args);

void* handle_new_message_requests(void* args);

void connect_user(struct server* server, long pid, char username[USERNAME_LEN]);

void disconnect_user(struct server* server, long pid, char username[USERNAME_LEN]);

void add_message(struct server* server, struct message_msg message);

void delete_message(struct server* server, int index);

void send_message_msg();

void send_user_msg(struct server* server, char username[USERNAME_LEN], enum connection_type type, int start, int end);

int validate_user(struct user** users, int users_size, long pid, char username[USERNAME_LEN]);

#endif // !SERVER_H
