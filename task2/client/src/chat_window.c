#include "../headers/chat_window.h"
#include <curses.h>

struct chat_window* create_chat_window(int height, int width, int start_y, int start_x) {
  struct chat_window* chat_window = (struct chat_window*) malloc(sizeof(struct chat_window));
  
  chat_window->window = newwin(height, width, start_y, start_x);
  chat_window->listbox = create_listbox(height - 2, width - 2, start_y + 1, start_x + 1);
  
  keypad(chat_window->window, TRUE);
  draw_border(chat_window);
  return chat_window;
}

int process_chat_window_input(struct chat_window* chat_window) {
  int c;

  while ((c = wgetch(chat_window->window)) != KEY_F(1)) {
    switch (c) {
      case KEY_DOWN:
        move_chat_down(chat_window);
        break;
      case KEY_UP:
        move_chat_up(chat_window);
        break;
      // Tab
      case 9:
        return 1;
    }
    wrefresh(chat_window->window);
  }

  return 0;
}

void draw_border(struct chat_window* chat_window) {
  box(chat_window->window, 0, 0);
  wrefresh(chat_window->window);
}

void add_message(struct chat_window* chat_window, char* sender, char* message) {
  char* buffer = (char*) malloc(ITEM_LENGTH * sizeof(char));
  snprintf(buffer, ITEM_LENGTH, "%s: %s", sender, message);
  
  if (add_item(chat_window->listbox, buffer) != 0) {
    // delete latest message
    delete_item(chat_window->listbox, 0);
    add_item(chat_window->listbox, buffer);
  }
  
  /*if (chat_window->listbox->length < chat_window->listbox->height / 2) {
    move_chat_down(chat_window);
  }*/
  
  free(buffer);
}

void move_chat_up(struct chat_window* chat_window) {
  move_up(chat_window->listbox);
  update(chat_window->listbox);
}

void move_chat_down(struct chat_window* chat_window) {
  move_down(chat_window->listbox);
  update(chat_window->listbox);
}

void dispose_chat_window(struct chat_window* chat_window) {
  dispose(chat_window->listbox);
  delwin(chat_window->window);
  free(chat_window);
}

