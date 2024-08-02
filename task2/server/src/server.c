#include "../headers/server.h"

/**
 * create_server - used to create server object. Mallocs
 * memory for collections, opens queues, inits mutexes.
 * 
 * Return: pointer to object of server struct 
 */
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

/**
 * run_server - used to run previously created server,
 * creates thread for new_messages_queue, calls
 * connection requests handler.
 * @server - pointer to an object of server struct 
 */
void run_server(struct server* server) {
  fprintf(stderr, "Server started.\n");   
  if (pthread_create(&server->messages_thread, NULL, handle_new_message_requests, (void*) server) != 0) {
    perror("pthread_create error");
    exit(EXIT_FAILURE);
  }
  fprintf(stderr, " -- Thread for new messages created.\n");
  handle_connection_requests(server);
}

int open_queue(char* filename, int id) {
  key_t queue_key;
  int queue;
  
  if ((queue_key = ftok(filename, id)) == -1) {
    perror("ftok error"); 
    exit(EXIT_FAILURE);
  }
  
  fprintf(stderr, "queue_key: %d\n", queue_key);
  if ((queue = msgget(queue_key, IPC_CREAT | 0666)) == -1) {
    perror("msgget error"); 
    exit(EXIT_FAILURE);
  }
  
  return queue;
}

/**
 * handle_connection_requests - used to receive conenction requests
 * from login_queue, notifies users about new connection or disconnect,
 * sends users and messages to new user. Runs in main thread, can be run
 * in child thread.
 * @args - pointer to an object of server struct, must be cast manually
 *
 * Return: NULL on exit
 */
void* handle_connection_requests(void* args) {
  struct server* server = (struct server*) args;
  struct connection_msg message;
  
  while (1) {
    fprintf(stderr, "Waiting for connection messages.\n");
    if (msgrcv(server->login_queue, &message, sizeof(struct connection_request), 0, 0) != -1) {
      switch (message.request.type) {
        case CONNECT:
          fprintf(stderr, " -- User is trying to connect\n");
          if (connect_user(server, message.mtype, message.request.username) == -1) {
            break;
          }
          send_all_users(server, message.mtype);
          send_all_messages(server, message.mtype);
          send_user_ranged(server, message.request.username, message.request.type, 0, server->users_size - 1);
          break;
        case DISCONNECT:
          fprintf(stderr, " -- User is trying to disconnect\n");
          disconnect_user(server, message.mtype, message.request.username);
          send_user_ranged(server, message.request.username, message.request.type, 0, server->users_size);
          break;
        default:
          break;
      } 
    }
  }

  return NULL;
}

/**
 * handle_new_message_requests - used to receive new message requests
 * from new_messages_queue, adds message to collection, notifies all users 
 * about new message.
 * @args - pointer to an object of server struct, must be cast manually
 *
 * Return: NULL on exit 
 */
void* handle_new_message_requests(void* args) {
  struct server* server = (struct server*) args;
  struct message_msg message;
  
  while (1) {
    fprintf(stderr, "Waiting for new message requests\n");
    if (msgrcv(server->new_messages_queue, &message, sizeof(struct message_msg), 0, 0) != -1) {
      fprintf(stderr, "New message received: %s %s\n", message.request.username, message.request.message);
      add_message(server, message);
      send_message_ranged(server, message.request.username, message.request.message, 0, server->users_size);
    }
  }

  return NULL;
}

/**
 * send_user_msg - used to send user requests to specific user with pid.
 * @server - pointer to an object of server struct
 * @username - username of receiver
 * @type - type of connection: CONNECT or DISCONNECT 
 * @pid - pid of receiver  
 */
void send_user_msg(struct server* server, char username[USERNAME_LEN], enum connection_type type, long pid) {
  struct user_msg message;
  
  message.request.type = type;
  strncpy(message.request.username, username, USERNAME_LEN);
  message.mtype = pid; 

  if (msgsnd(server->users_queue, &message, sizeof(struct user_request), 0) == -1) {
    perror("msgsnd error");
    exit(EXIT_FAILURE);
  } 
}

/**
 * send_message_msg - used to send message requests to specific user with pid.
 * @server - pointer to an object of server struct 
 * @username - username of receiver
 * @text - message text 
 * @pid - pid of receiver
 */
void send_message_msg(struct server* server, char username[USERNAME_LEN], char text[MESSAGE_LEN], long pid) {
  struct message_msg message;

  strncpy(message.request.username, username, USERNAME_LEN);
  strncpy(message.request.message, text, MESSAGE_LEN);
  message.mtype = pid; 

  if (msgsnd(server->messages_queue, &message, sizeof(struct message_request), 0) == -1) {
    perror("msgsnd error");
    exit(EXIT_FAILURE);
  } 
}

/**
 * send_all_messages - used to send all messages to specific user with pid.
 * @server - pointer to an object of server struct
 * @pid - pid of receiver
 */
void send_all_messages(struct server* server, long pid) {
  pthread_mutex_lock(&server->messages_mutex);
  for (int i = 0; i < server->messages_size; i++) {
    send_message_msg(server, server->messages[i]->username, server->messages[i]->text, pid); 
  }
  pthread_mutex_unlock(&server->messages_mutex);
}

/**
 * send_all_users - used to send all connected users to specific user with pid.
 * @server - pointer to an object of server struct 
 * @pid - pid of receiver
 */
void send_all_users(struct server* server, long pid) {
  pthread_mutex_lock(&server->users_mutex);
  for (int i = 0; i < server->users_size; i++) {
    send_user_msg(server, server->users[i]->username, CONNECT, pid); 
  }
  pthread_mutex_unlock(&server->users_mutex);
}

/**
 * send_message_ranged - used to send message request to users in range from start
 * to end in users collection.
 * @server - pointer to an object of server struct
 * @username - username of message sender
 * @text - message of sender
 * @start - index of first receiver
 * @end - index of last receiver (not included) 
 */
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

/**
 * send_users_ranged - used to send user request to users in range from start
 * to end in users collection.
 * @server - pointer to an object of server struct
 * @username - username of message sender
 * @type - user conenction type 
 * @start - index of first receiver
 * @end - index of last receiver (not included) 
 */
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

/**
 * connect_user - used to add user to collection from connection_msg.
 * Message type is pid of receiver. If room is full sends disconnect request.
 * Otherwise adds user to users collection.
 * @server - pointer to an object of server struct
 * @pid - pid of sender
 * @username - username of sender 
 *
 * Return: 0 if successful, -1 if room is full   
 */
int connect_user(struct server* server, long pid, char username[USERNAME_LEN]) {
  pthread_mutex_lock(&server->users_mutex);
  if (server->users_size == MAX_USERS) {
    struct connection_msg message;
    message.mtype = pid;
    message.request.type = DISCONNECT;
    strncpy(message.request.username, username, USERNAME_LEN);
    
    if (msgsnd(server->login_queue, &message, sizeof(message.request), 0) == -1) {
      perror("msgsnd error");
      exit(EXIT_FAILURE);
    }
    return -1;
  }
     
  struct user* user = (struct user*) malloc(sizeof(struct user));
  user->pid = pid;
  strncpy(user->username, username, strlen(username));
  server->users[server->users_size] = user;
  server->users_size++;
  pthread_mutex_unlock(&server->users_mutex);
  fprintf(stderr, "-- User connected\n");
  return 0;
}

/**
 * disconnect_user - used to delete user from users collection.
 * Called when connection request with DISCONNECT type received.
 * @server - pointer to an object of server struct 
 * @pid - pid of disconnected user
 * @username - username of disconnected user
 */
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

/**
 * add_message - used to add message from message request to collection.
 * If overflow occurs delets latest message and adds a new one. Validates
 * pid and username of user.
 * @server - pointer to an object of server struct
 * @message - pointer to an object of message_msg struct
 */
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
  struct message* msg = (struct message*) malloc(sizeof(struct message));
  strncpy(msg->username, message.request.username, USERNAME_LEN);
  strncpy(msg->text, message.request.message, USERNAME_LEN); 
  server->messages[server->messages_size] = msg;
  server->messages_size++;
  pthread_mutex_unlock(&server->messages_mutex);
}

/**
 * delete_message - used to delete message from messages
 * collection by index. Moves messages from index + 1 to left,
 * decrements size.
 * @server - pointer to an object of server struct
 * @index - position of message to delete
 */
void delete_message(struct server* server, int index) {
  
  pthread_mutex_lock(&server->messages_mutex);

  for (int i = index; i < server->users_size - 1; i++) {
    server->users[index] = server->users[i + 1]; 
  }

  server->users_size--;
  pthread_mutex_unlock(&server->messages_mutex);
}

/**
 * validate_user - used to validate receiver user data. Checks
 * if user with pid and username exists.
 * @users - pointer to a collection of users
 * @users_size - size of users collection
 * @pid - pid of sender
 * @username - username of sender
 *
 * Return: index of user if successful, -1 if user not exists 
 */
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

/**
 * free_server - used to free memory allocated for server.
 * Frees collection, closes queues, destroys mutexes. 
 * @server - pointer to an object of server struct.
 */ 
void free_server(struct server* server) {
  /* free users */ 
  for (int i = 0; i < server->users_size; i++) {
    free(server->users[i]);
  }
  free(server->users);
  
  /* free messages */
  for (int i = 0; i < MAX_CHAT_MESSAGES; i++) {
    free(server->messages[i]);
  }
  free(server->messages);
  
  msgctl(server->login_queue, IPC_RMID, NULL);
  msgctl(server->messages_queue, IPC_RMID, NULL);  
  msgctl(server->users_queue, IPC_RMID, NULL);  
  msgctl(server->new_messages_queue, IPC_RMID, NULL);  
  
  pthread_cancel(server->messages_thread);
  /* destroy mutexes */
  pthread_mutex_destroy(&server->users_mutex);
  pthread_mutex_destroy(&server->messages_mutex);  
  free(server); 
}
