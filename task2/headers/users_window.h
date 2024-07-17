#ifndef USERS_WINDOW_H
#define USERS_WINDOW_H

#include <curses.h>
#include <stdio.h>
#include "listbox.h"

struct users_window {
  WINDOW* window;
  struct listbox* listbox;
};

struct users_window* create_users_window(int width, int height, int start_y, int start_x);

void draw_users_border(struct users_window* users_window);

void add_user(struct users_window* users_window, char* user);

void move_users_up(struct users_window* users_window);

void move_users_down(struct users_window* users_window);

void dispose_users_window(struct users_window* window);

#endif // USERS_WINDOW_H
