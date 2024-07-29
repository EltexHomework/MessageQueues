#include "../headers/chat.h"
#include <curses.h>

void get_username(char *username, size_t len) {
  printw("Enter username: ");
  refresh();
  
  getnstr(username, len - 1);
  
  username[strcspn(username, "\n")] = '\0';
  
}

int main(void) {
  initscr();
  start_color();
  cbreak();
  echo();
  keypad(stdscr, TRUE);

  char username[USERNAME_LEN];

  get_username(username, USERNAME_LEN);

  noecho();
  int width, height;
  getmaxyx(stdscr, height, width);

  struct chat* chat = create_chat(username, height, width, 0, 0);
  run_chat(chat);

  endwin();
  return 0;
}
