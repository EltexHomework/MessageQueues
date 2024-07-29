#include "../headers/chat.h"
#include <string.h>
#include <sys/msg.h>

struct chat* create_chat(char* username, int height, int width, int start_y, int start_x) {
  struct chat* chat = (struct chat*) malloc(sizeof(struct chat));
  int chat_window_height = height - 4; 
  int chat_window_width = width - 30; 

  int users_window_height = height - 4; 
  int users_window_width = 30; 

  int input_field_height = 4; 
  int input_field_width = width; 

  chat->chat_window = create_chat_window(chat_window_height, chat_window_width, start_y, start_x);
  chat->users_window = create_users_window(users_window_height, users_window_width, start_y, start_x + chat_window_width);
  chat->input_field = create_field(input_field_height, input_field_width, start_y + chat_window_height, start_x);
  chat->current_window = 1;
  chat->pid = (long) getpid();
  chat->login_queue = open_queue(SERVER_QUEUE, LOGIN_QUEUE_ID);
  chat->messages_queue = open_queue(SERVER_QUEUE, UPDATE_MESSAGES_QUEUE_ID);
  chat->users_queue = open_queue(SERVER_QUEUE, UPDATE_USERS_QUEUE_ID);
  chat->new_messages_queue = open_queue(SERVER_QUEUE, NEW_MESSAGES_QUEUE_ID);

  strncpy(chat->username, username, USERNAME_LEN);
  update(chat->users_window->listbox);
  update(chat->chat_window->listbox);

  return chat;
}

void run_chat(struct chat* chat) {
  pthread_t users_thread;
  pthread_t message_thread;
  
  connect_to_server(chat);
  
  if (pthread_create(&message_thread, NULL, handle_message_requests, chat) != 0) {
    perror("pthread_create error");
    exit(EXIT_FAILURE);
  }
  
  if (pthread_create(&users_thread, NULL, handle_user_requests, chat) != 0) {
    perror("pthread_create error");
    exit(EXIT_FAILURE);
  }
  

  while (1) {
    // chat
    if (chat->current_window == 1) {
      if (process_chat_window_input(chat->chat_window) == 1) {
        chat->current_window++; 
      } else {
        break;
      }
    }
    // users
    else if (chat->current_window == 2) {
      if (process_users_window_input(chat->users_window) == 1) {
        chat->current_window++; 
      } else {
        break;
      }
    } 
    // input_field
    else {
      int result;
      if ((result = process_field_input(chat->input_field)) == 1) {
        chat->current_window = 1;
      } else if (result == 2) {
        send_message(chat);         
      } else {
        break;
      }
    }
  }

  disconnect_from_server(chat);  
  dispose_chat(chat);
  endwin();
}

void connect_to_server(struct chat* chat) {
  struct connection_msg message;
  
  message.mtype = chat->pid;
  message.request.type = CONNECT;
  strncpy(message.request.username, chat->username, USERNAME_LEN);
  
  if (msgsnd(chat->login_queue, &message, sizeof(struct connection_request), 0) == -1) {
    perror("msgsnd error");
    exit(EXIT_FAILURE);
  }
 
  /*while(1) {
    if (msgrcv(chat->login_queue, &message, sizeof(message.request), chat->pid, 0) != -1) {
      break;
    }
  }
  
  if (message.request.type == DISCONNECT) {
    perror("connect error");
    exit(EXIT_FAILURE);
  }*/
}

void disconnect_from_server(struct chat* chat) {
  struct connection_msg message;

  message.mtype = chat->pid;
  message.request.type = DISCONNECT;
  strncpy(message.request.username, chat->username, USERNAME_LEN);

  if (msgsnd(chat->login_queue, &message, sizeof(struct connection_request), 0) == -1) {
    perror("msgsnd error");
    exit(EXIT_FAILURE);
  }
}

void* handle_user_requests(void* args) {
  struct chat* chat = (struct chat*) args;
  struct user_msg message;

  while (1) {
    if (msgrcv(chat->users_queue, &message, sizeof(struct user_request), chat->pid, 0) != -1) {
      switch (message.request.type) {
        case CONNECT:
          add_user(chat->users_window, message.request.username);
          update(chat->users_window->listbox);
          break;
        case DISCONNECT:
          delete_user(chat->users_window, message.request.username); 
          update(chat->users_window->listbox);
          break;
        default:
          break;
      }     
    }
  }

  return NULL;
}

void* handle_message_requests(void* args) {
  struct chat* chat = (struct chat*) args;
  struct message_msg message;

  while (1) {
    if (msgrcv(chat->messages_queue, &message, sizeof(struct message_request), chat->pid, 0) != -1) {
      add_message(chat->chat_window, message.request.username, message.request.message);
      update(chat->chat_window->listbox);
    }  
  }

  return NULL;
}

int open_queue(char* filename, int id) {
  key_t queue_key;
  int queue;

  if ((queue_key = ftok(filename, id)) == -1) {
    perror("ftok error"); 
    exit(EXIT_FAILURE);
  }
  fprintf(stderr, "queue_key: %d\n", queue_key);

  if ((queue = msgget(queue_key, 0666)) == -1) {
    perror("msgget error"); 
    exit(EXIT_FAILURE);
  }
  
  return queue;
}

void send_message(struct chat* chat) {
  struct message_msg message;
  char* str = get_str(chat->input_field);
  message.mtype = chat->pid;
  strncpy(message.request.username, chat->username, USERNAME_LEN);
  strncpy(message.request.message, str, MESSAGE_LEN);

  if (msgsnd(chat->new_messages_queue, &message, sizeof(message.request), 0) == -1) {
    perror("msgsnd error");
    exit(EXIT_FAILURE);
  }
}

void dispose_chat(struct chat* chat) {
  dispose_chat_window(chat->chat_window);
  dispose_users_window(chat->users_window);
  dispose_field(chat->input_field);
  free(chat);
}

