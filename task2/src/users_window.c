#include "../headers/users_window.h"

struct users_window* create_users_window(int height, int width, int start_y, int start_x) {
  struct users_window* users_window = (struct users_window*) malloc(sizeof(struct users_window));
  
  users_window->window = newwin(height, width, start_y, start_x);
  users_window->listbox = create_listbox(height - 2, width - 2, start_y + 1, start_x + 1);
  
  draw_users_border(users_window);
  return users_window;
}

void draw_users_border(struct users_window* users_window) {
  box(users_window->window, 0, 0);
  wrefresh(users_window->window);
}

void add_user(struct users_window* users_window, char* user) {
  if (add_item(users_window->listbox, user) != 0) {
    increase_size(users_window->listbox, 1); 
    add_item(users_window->listbox, user);
  }
  
  /*if (users_window->listbox->length < users_window->listbox->height / 2) {
    move_users_down(users_window);
  }*/
}

void move_users_up(struct users_window* users_window) {
  move_up(users_window->listbox);
  update(users_window->listbox);
}

void move_users_down(struct users_window* users_window) {
  move_down(users_window->listbox);
  update(users_window->listbox);
}

void dispose_users_window(struct users_window* users_window) {
  dispose(users_window->listbox);
  delwin(users_window->window);
  free(users_window);
}


