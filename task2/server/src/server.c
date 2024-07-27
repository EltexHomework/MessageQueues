#include "../headers/server.h"
#include <pthread.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

struct server* create_server() {
  struct server* server = (struct server*) malloc(sizeof(struct server));
  
  server->users = (struct user**) malloc(MAX_USERS * sizeof(struct user*));  
  server->messages = (struct message**) malloc(MAX_CHAT_MESSAGES * sizeof(struct message*));
  server->users_size = 0;
  server->messages_size = 0;
  server->login_queue = open_queue(SERVER_QUEUE, LOGIN_QUEUE_ID); 
  server->messages_queue = open_queue(SERVER_QUEUE, UPDATE_MESSAGES_QUEUE_ID);
  server->users_queue = open_queue(SERVER_QUEUE, UPDATE_USERS_QUEUE_ID);
  server->new_messages_queue = open_queue(SERVER_QUEUE, NEW_MESSAGES_QUEUE_ID);
  pthread_mutex_init(&server->users_mutex, NULL);
  pthread_mutex_init(&server->messages_mutex, NULL);

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

int open_queue(char* filename, int id) {
  key_t queue_key;
  int queue;

  if ((queue_key = ftok(filename, id) == -1)) {
    perror("ftok error"); 
    exit(EXIT_FAILURE);
  }
  
  if ((queue = msgget(queue_key, IPC_CREAT | 0666)) == -1) {
    perror("msgget error"); 
    exit(EXIT_FAILURE);
  }
  
  return queue;
}

// Handlers
void* handle_connection_requests(void* args) {
  struct server* server = (struct server*) args;
  struct connection_msg message;

  while (1) {
    if (msgrcv(server->login_queue, &message, sizeof(message.request), 0, 0) != -1) {
      switch (message.request.type) {
        case CONNECT:
          connect_user(server, message.mtype, message.request.username);
          send_all_users(server, message.mtype);
          send_all_messages(server, message.mtype);
          send_user_ranged(server, message.request.username, message.request.type, 0, server->users_size - 1);
          break;
        case DISCONNECT:
          disconnect_user(server, message.mtype, message.request.username);
          send_user_ranged(server, message.request.username, message.request.type, 0, server->users_size - 1);
          break;
        default:
          break;
      } 
    }
  }

  return NULL;
}

void* handle_new_message_requests(void* args) {
  struct server* server = (struct server*) args;
  struct message_msg message;

  while (1) {
    if (msgrcv(server->login_queue, &message, sizeof(message.request), 0, 0) != -1) {
      add_message(server, message);
      send_message_ranged(server, message.request.username, message.request.message, 0, server->users_size);
    }
  }

  return NULL;
}

// Single receiver
void send_user_msg(struct server* server, char username[USERNAME_LEN], enum connection_type type, long pid) {
  struct user_msg message;
  
  message.request.type = type;
  strncpy(message.request.username, username, USERNAME_LEN);
  message.mtype = pid; 

  if (msgsnd(server->users_queue, &message, sizeof(message.request), 0) == -1) {
    perror("msgsnd error");
    exit(EXIT_FAILURE);
  } 
}

void send_message_msg(struct server* server, char username[USERNAME_LEN], char text[MESSAGE_LEN], long pid) {
  struct message_msg message;

  strncpy(message.request.username, username, USERNAME_LEN);
  strncpy(message.request.message, text, MESSAGE_LEN);
  message.mtype = pid; 

  if (msgsnd(server->messages_queue, &message, sizeof(message.request), 0) == -1) {
    perror("msgsnd error");
    exit(EXIT_FAILURE);
  } 
}

void send_all_messages(struct server* server, long pid) {
  pthread_mutex_lock(&server->messages_mutex);
  for (int i = 0; i < server->messages_size; i++) {
    send_message_msg(server, server->messages[i]->username, server->messages[i]->text, pid); 
  }
  pthread_mutex_unlock(&server->messages_mutex);
}

void send_all_users(struct server* server, long pid) {
  pthread_mutex_lock(&server->users_mutex);
  for (int i = 0; i < server->users_size; i++) {
    send_user_msg(server, server->users[i]->username, CONNECT, pid); 
  }
  pthread_mutex_unlock(&server->users_mutex);
}

// Multiple receivers
void send_message_ranged(struct server* server, char username[USERNAME_LEN], char text[MESSAGE_LEN], int start, int end) {
  if (end > server->users_size) {
    return;
  }
  
  pthread_mutex_lock(&server->users_mutex);
  for (int i = start; i < end; i++) {
    long pid = server->users[i]->pid;
    send_message_msg(server, username, text, pid);  
  }
  pthread_mutex_unlock(&server->users_mutex);
}

void send_user_ranged(struct server* server, char username[USERNAME_LEN], enum connection_type type, int start, int end) {
  if (end > server->users_size) {
    return;
  }
  
  pthread_mutex_lock(&server->users_mutex);
  for (int i = start; i < end; i++) {
    long pid = server->users[i]->pid;
    send_user_msg(server, username, type, pid);
  }
  pthread_mutex_unlock(&server->users_mutex);
}

// Users operations
void connect_user(struct server* server, long pid, char username[USERNAME_LEN]) {
  pthread_mutex_lock(&server->users_mutex);
  if (server->users_size == MAX_USERS) {
    return;
  }
     
  struct user* user = server->users[server->users_size];
  user->pid = pid;
  strncpy(user->username, username, strlen(username));
  server->users_size++;
  pthread_mutex_unlock(&server->users_mutex);
}

void disconnect_user(struct server* server, long pid, char username[USERNAME_LEN]) {
  int index;

  pthread_mutex_lock(&server->users_mutex);
  if ((index = validate_user(server->users, server->users_size, pid, username)) == -1) {
    return; 
  }
  
  for (int i = index; i < server->users_size - 1; i++) {
    server->users[index] = server->users[i + 1]; 
  }

  server->users_size--;
  pthread_mutex_unlock(&server->users_mutex); 
}

// Messages operations
void add_message(struct server* server, struct message_msg message) {

  pthread_mutex_lock(&server->users_mutex);
  if (validate_user(server->users, server->users_size, message.mtype, message.request.username) == -1) {
    return;
  }
  pthread_mutex_unlock(&server->users_mutex);

  pthread_mutex_lock(&server->messages_mutex);
  if (server->messages_size == MAX_CHAT_MESSAGES) {
    delete_message(server, 0);
  }
  struct message* msg = server->messages[server->messages_size];
  strncpy(msg->username, message.request.username, USERNAME_LEN);
  strncpy(msg->text, message.request.message, USERNAME_LEN); 
  pthread_mutex_unlock(&server->messages_mutex);
}

void delete_message(struct server* server, int index) {
  pthread_mutex_lock(&server->messages_mutex);

  for (int i = index; i < server->users_size - 1; i++) {
    server->users[index] = server->users[i + 1]; 
  }

  server->users_size--;
  pthread_mutex_unlock(&server->messages_mutex);
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

void free_server(struct server* server) {
  // Free users 
  for (int i = 0; i < MAX_USERS; i++) {
    free(server->users[i]);
  }
  free(server->users);
  
  // Free messages
  for (int i = 0; i < MAX_CHAT_MESSAGES; i++) {
    free(server->messages[i]);
  }
  free(server->messages);
  
  // Destroy threads
  pthread_mutex_destroy(&server->users_mutex);
  pthread_mutex_destroy(&server->messages_mutex);  
}
