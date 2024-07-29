#include "../headers/chat.h"

int main(void) {
  initscr();
  start_color();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
    
  struct chat* chat = create_chat("TheNorth");
  run_chat(chat);
  return 0;
}
