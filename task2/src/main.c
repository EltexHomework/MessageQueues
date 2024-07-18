#include "../headers/chat.h"
#include <curses.h>
#include <stdio.h>

int main(void) {
  initscr();
  start_color();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
    
  struct chat* chat = create_chat();
  run_chat(chat);
  return 0;
}
