#include "../headers/chat.h"
#include <curses.h>

void get_username(char *username, size_t len) {
  printw("Enter username: ");
  refresh();
  
  getnstr(username, len - 1);
  
  username[strcspn(username, "\n")] = '\0';
  
}

int main(void) {
  int width, height;
  char username[USERNAME_LEN];

  initscr();
  start_color();
  cbreak();
  echo();
  keypad(stdscr, TRUE);
  getmaxyx(stdscr, height, width);

  get_username(username, USERNAME_LEN);
  noecho();

  struct chat* chat = create_chat(username, height, width, 0, 0);
  run_chat(chat);

  endwin();
  return 0;
}
