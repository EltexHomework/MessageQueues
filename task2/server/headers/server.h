#ifndef SERVER_H
#define SERVER_H

#include "../headers/config.h"
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

int open_queue(char* filename, int id);

// Handlers for threads
void* handle_connection_requests(void* args);

void* handle_new_message_requests(void* args);

// Users operation
void connect_user(struct server* server, long pid, char username[USERNAME_LEN]);

void disconnect_user(struct server* server, long pid, char username[USERNAME_LEN]);

// Messages operation 
void add_message(struct server* server, struct message_msg message);

void delete_message(struct server* server, int index);

// Ranged amount of receivers
void send_message_ranged(struct server* server, char username[USERNAME_LEN], char text[MESSAGE_LEN], int start, int end); 

void send_user_ranged(struct server* server, char username[USERNAME_LEN], enum connection_type type, int start, int end);

// Single receiver
void send_user_msg(struct server* server, char username[USERNAME_LEN], enum connection_type type, long pid);
  
void send_message_msg(struct server* server, char username[USERNAME_LEN], char text[MESSAGE_LEN], long pid);

void send_all_messages(struct server* server, long pid);

void send_all_users(struct server* server, long pid);

int validate_user(struct user** users, int users_size, long pid, char username[USERNAME_LEN]);

void free_server(struct server* server);
#endif // !SERVER_H
