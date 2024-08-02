#ifndef SERVER_H
#define SERVER_H

#include "../headers/config.h"
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <pthread.h>
#include <complex.h>
#include <string.h>
#include <stdio.h>
#include "../../requests/headers/users_msg.h"
#include "../../requests/headers/connection_msg.h"
#include "../../requests/headers/message_msg.h"
#include "user.h"
#include "message.h"

/*
 * Server struct implements functionality for
 * messanger server with one room, capable of
 * connecting and disconnecting users. Message
 * queue are used to transfer messages between
 * server and client.
 *
 * [Example]
 * struct server* server;
 *
 * void free_memory();
 *
 * int main(void) {
 *  server = create_server();
 *  run_server(server); 
 *  return 0;  
 * }
 * 
 * void free_memory() {
 *  free_server(server); 
 * }
 */
struct server {
  struct user** users;
  struct message** messages;
  pthread_mutex_t users_mutex;
  pthread_mutex_t messages_mutex;

  int users_size;
  int messages_size;

  /* mq for connection messages */ 
  int login_queue;
  /* mq for new message users notification */ 
  int messages_queue;
  /* mq for user connection or disconnection notification */
  int users_queue;
  /* mq for newly send messages */
  int new_messages_queue;
};

struct server* create_server();

void run_server(struct server* server);

int open_queue(char* filename, int id);

void* handle_connection_requests(void* args);

void* handle_new_message_requests(void* args);

int connect_user(struct server* server, long pid, char username[USERNAME_LEN]);

void disconnect_user(struct server* server, long pid, char username[USERNAME_LEN]);

void add_message(struct server* server, struct message_msg message);

void delete_message(struct server* server, int index);

void send_message_ranged(struct server* server, char username[USERNAME_LEN], char text[MESSAGE_LEN], int start, int end); 

void send_user_ranged(struct server* server, char username[USERNAME_LEN], enum connection_type type, int start, int end);

void send_user_msg(struct server* server, char username[USERNAME_LEN], enum connection_type type, long pid);
  
void send_message_msg(struct server* server, char username[USERNAME_LEN], char text[MESSAGE_LEN], long pid);

void send_all_messages(struct server* server, long pid);

void send_all_users(struct server* server, long pid);

int validate_user(struct user** users, int users_size, long pid, char username[USERNAME_LEN]);

void free_server(struct server* server);
#endif // !SERVER_H
