#include "../headers/chat.h"

/**
 * create_chat - creates chat struct object, calculated width
 * and height of windows, opens queues.
 * @username: nickname of user
 * @height: height of chat window
 * @width: width of chat window
 * @start_y: y position from where chat window is drawn
 * @start_x: x position from where chat window is drawn
 *
 * Return: pointer to struct chat if successful, or NULL as error code. 
 */ 
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
  
  if ((chat->login_queue = open_queue(SERVER_QUEUE, LOGIN_QUEUE_ID)) == -1) {
    return NULL;
  }
  if ((chat->messages_queue = open_queue(SERVER_QUEUE, UPDATE_MESSAGES_QUEUE_ID)) == -1) {
    return NULL;
  }
  if ((chat->users_queue = open_queue(SERVER_QUEUE, UPDATE_USERS_QUEUE_ID)) == -1) {
    return NULL;
  }
  if ((chat->new_messages_queue = open_queue(SERVER_QUEUE, NEW_MESSAGES_QUEUE_ID)) == -1) {
    return NULL;
  }

  strncpy(chat->username, username, USERNAME_LEN);
  update(chat->users_window->listbox);
  update(chat->chat_window->listbox);

  return chat;
}
/**
 * run_chat - used to establish connection to server
 * and run chat functionality.
 * @chat - pointer to chat struct object
 *
 * Return: 
 */
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
    /* windows focus on chat_window */
    if (chat->current_window == 1) {
      if (process_chat_window_input(chat->chat_window) == 1) {
        chat->current_window++; 
      } else {
        break;
      }
    }
    /* windows focus on users_window */
    else if (chat->current_window == 2) {
      if (process_users_window_input(chat->users_window) == 1) {
        chat->current_window++; 
      } else {
        break;
      }
    } 
    /* windows focus on input_field */
    else {
      int result;
      if ((result = process_field_input(chat->input_field)) == 1) {
        chat->current_window = 1;
      }
      /* received Enter key */
      else if (result == 2) {
        send_message(chat);         
      } 
      else {
        break;
      }
    }
  }

  disconnect_from_server(chat);  
  dispose_chat(chat);
  endwin();
}

/**
 * connect_to_server - used to establish connection
 * to server, which config specified in config.h.
 * connection_msg struct is used as a message for message
 * queue.
 * @chat - pointer to chat struct object
 */
void connect_to_server(struct chat* chat) {
  struct connection_msg message;
  
  message.mtype = chat->pid;
  message.request.type = CONNECT;
  strncpy(message.request.username, chat->username, USERNAME_LEN);
  
  if (msgsnd(chat->login_queue, &message, sizeof(struct connection_request), 0) == -1) {
    perror("msgsnd error");
    exit(EXIT_FAILURE);
  }
}

/**
 * disconnect_from_server - used to send disconnect message
 * to server.
 * @chat - pointer to chat struct object
 */
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

/**
 * handle_user_requests - handler for user_queue message of
 * type user_msg. Decides to connect or to disconnect user 
 * depending on message_msg.request.type of type connection_type.
 * Can be used in thread. Currently running in main thread.
 * @args - pointer to chat struct, must be cast manually
 */
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

/**
 * handle_message_requests - handler for message_queue messages of
 * type message_msg. Currently running in child thread.
 * @args - pointer to chat struct, must be cast manually
 */
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

/**
 * open_queue - used to open queue by filename and id.
 * @filename - name of file that queue is opened on  
 * @id - identifier that splits different queues on same file
 * 
 * Return: queue id if successful, or -1 error code
 */
int open_queue(char* filename, int id) {
  key_t queue_key;
  int queue;

  if ((queue_key = ftok(filename, id)) == -1) {
    perror("ftok error");
    return -1;
  }

  if ((queue = msgget(queue_key, 0666)) == -1) {
    perror("msgget error");
    return -1;
  }
  
  return queue;
}

/**
 * send_message - used to send message to server of type
 * message_msg.
 * @chat - pointer to chat struct object
 */
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

/**
 * dispose_chat - used to free memory allocated for chat.
 * Should be called in onexit function to free memory on SIGINT
 * signal.
 * @chat - pointer to chat struct object
 */
void dispose_chat(struct chat* chat) {
  dispose_chat_window(chat->chat_window);
  dispose_users_window(chat->users_window);
  dispose_field(chat->input_field);
  free(chat);
}

