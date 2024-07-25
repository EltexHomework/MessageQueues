#include "../headers/server.h"
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

struct server* create_server() {
  struct server* server = (struct server*) malloc(sizeof(struct server));
  server->users = (struct user**) malloc(MAX_USERS * sizeof(struct user*));  
  server->messages = (struct message**) malloc(sizeof(struct message*));

  key_t login_queue_key;
  key_t users_queue_key;
  key_t messages_queue_key;
  key_t new_messages_queue_key;
  
  if ((login_queue_key = ftok(SERVER_QUEUE, LOGIN_QUEUE_ID) == -1)) {
    perror("lq ftok error"); 
    exit(EXIT_FAILURE);
  } 
  if ((users_queue_key = ftok(SERVER_QUEUE, UPDATE_USERS_QUEUE_ID) == -1)) {
    perror("uuq ftok error"); 
    exit(EXIT_FAILURE);
  }
  if ((messages_queue_key = ftok(SERVER_QUEUE, UPDATE_MESSAGES_QUEUE_ID) == -1)) {
    perror("umq ftok error"); 
    exit(EXIT_FAILURE);
  }
  if ((new_messages_queue_key = ftok(SERVER_QUEUE, NEW_MESSAGES_QUEUE_ID) == -1)) {
    perror("nmq ftok error"); 
    exit(EXIT_FAILURE);
  }

  if ((server->login_queue = msgget(login_queue_key, IPC_CREAT | 0666)) == -1) {
    perror("lq msgget error"); 
    exit(EXIT_FAILURE);
  }
  if ((server->users_queue = msgget(users_queue_key, IPC_CREAT | 0666)) == -1) {
    perror("lq msgget error"); 
    exit(EXIT_FAILURE);
  }
  if ((server->messages_queue = msgget(messages_queue_key, IPC_CREAT | 0666)) == -1) {
    perror("lq msgget error"); 
    exit(EXIT_FAILURE);
  }
  if ((server->new_messages_queue = msgget(new_messages_queue_key, IPC_CREAT | 0666)) == -1) {
    perror("lq msgget error"); 
    exit(EXIT_FAILURE);
  }
  
  return server;
}

void run_server(struct server* server) {
  pthread_t new_messages_thread;
  
  if (pthread_create(&new_messages_thread, NULL, handle_new_message_requests, (void*) server) != 0) {
    perror("pthread_create error");
    exit(EXIT_FAILURE);
  }
  handle_connection_requests(server);
}

void* handle_connection_requests(void* args) {
  struct server* server = (struct server*) args;
  struct connection_msg message;

  while (1) {
    if (msgrcv(server->login_queue, &message, sizeof(message.request), 0, 0) != -1) {
      switch (message.request.type) {
        case CONNECT:
          connect_user(server, message.mtype, message.request.username);
          send_user_msg(server, message.request.username, message.request.type, 0, server->users_size);
          break;
        case DISCONNECT:
          disconnect_user(server, message.mtype, message.request.username);
          send_user_msg(server, message.request.username, message.request.type, 0, server->users_size);
          break;
        default:
          break;
      } 
    }
  }

  return NULL;
}

void send_user_msg(struct server* server, char username[USERNAME_LEN], enum connection_type type, int start, int end) {
  struct user_msg message;
  message.request.type = type;
  strncmp(message.request.username, username, USERNAME_LEN);

  for (int i = start; i < end; i++) {
    message.mtype = server->users[i]->pid;
    msgsnd(server->users_queue, &message, sizeof(message.request), 0); 
  }
}

void* handle_new_message_requests(void* args) {
  struct server* server = (struct server*) args;
  struct message_msg message;

  while (1) {
    if (msgrcv(server->login_queue, &message, sizeof(message.request), 0, 0) != -1) {
      add_message(server, message);
      send_message_msg();
    }
  }

  return NULL;
}

void connect_user(struct server* server, long pid, char username[USERNAME_LEN]) {
  if (server->users_size == MAX_USERS) {
    return;
  }

  struct user* user = server->users[server->users_size];
  user->pid = pid;
  strncpy(user->username, username, strlen(username));
  server->users_size++;
}

void disconnect_user(struct server* server, long pid, char username[USERNAME_LEN]) {
  int index;
  if ((index = validate_user(server->users, server->users_size, pid, username)) == -1) {
    return; 
  }
  
  for (int i = index; i < server->users_size - 1; i++) {
    server->users[index] = server->users[i + 1]; 
  }

  server->users_size--;
}

void add_message(struct server* server, struct message_msg message) {
  if (validate_user(server->users, server->users_size, message.mtype, message.request.username) == -1) {
    return;
  }

  if (server->messages_size == MAX_CHAT_MESSAGES) {
    delete_message(server, 0);
  }
  struct message* msg = server->messages[server->messages_size];
  strncpy(msg->username, message.request.username, USERNAME_LEN);
  strncpy(msg->text, message.request.message, USERNAME_LEN); 
}

void delete_message(struct server* server, int index) {
  for (int i = index; i < server->users_size - 1; i++) {
    server->users[index] = server->users[i + 1]; 
  }

  server->users_size--;
}

int validate_user(struct user** users, int users_size, long pid, char username[USERNAME_LEN]) {
  for (int i = 0; i < users_size; i++) {
    if (users[i]->pid == pid) {
      if (strcmp(users[i]->username, username) == 0) {
        return i;
      } else {
        return -1;
      }
    } 
  }
  return -1;
}

