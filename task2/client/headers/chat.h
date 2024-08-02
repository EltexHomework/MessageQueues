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

/* Chat struct implements chat logic and connection
 * to server.
 * 
 * [Key bindings]
 * 
 * - F1 - used to exit program
 * 
 * [Example]
 * initscr();
 * cbreak();
 * noecho();
 * keypad(stdscr, TRUE);
 *
 * char username[USERNAME_LEN];
 *
 * get_username(username, USERNAME_LEN);
 *
 * int width, height;
 * getmaxyx(stdscr, height, width);
 *
 * struct chat* chat = create_chat(username, height, width, 0, 0);
 * run_chat(chat);
 *
 * endwin();
 */
struct chat {
  struct chat_window* chat_window;
  struct users_window* users_window;
  struct input_field* input_field;
  char username[USERNAME_LEN];

  int login_queue;
  int messages_queue;
  int users_queue;
  int new_messages_queue;
  
  /* pid of process that runs chat */
  long pid;
  
  /* specifies which window has focus on:
   * 1 - chat_window,
   * 2 - users_window,
   * 3 - input_field
   */
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
