#include "../headers/chat_window.h"
#include "../headers/users_window.h"
#include <curses.h>
#include <stdio.h>

int main(void) {
  initscr();
  start_color();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  struct chat_window* chat_window = create_chat_window(50, 160, 0, 0);
  struct users_window* users_window = create_users_window(50, 30, 0, 160);
  

  for (int i = 1; i < MAX_ITEMS + 128; i++) {
    char* sender = (char*) malloc(128 * sizeof(char));
    snprintf(sender, sizeof(sender), "%d", i);
    add_message(chat_window, sender, "asdasdasdasd");
    free(sender);
  }
  
  for (int i = 1; i < MAX_ITEMS + 128; i++) {
    char* sender = (char*) malloc(128 * sizeof(char));
    snprintf(sender, sizeof(sender), "%d", i);
    add_user(users_window, sender);
    free(sender);
  }

  update(users_window->listbox);
  update(chat_window->listbox);

  /*for (int i = 0; i < MAX_ITEMS; i++) {
    move_chat_down(chat_window);
  }*/

  for (int i = 0; i < MAX_ITEMS; i++) {
    move_users_down(users_window);
  }

  return 0;
}
