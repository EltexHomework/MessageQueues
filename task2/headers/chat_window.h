#ifndef CHAT_WINDOW_H
#define CHAT_WINDOW_H

#include <curses.h>
#include <stdio.h>
#include "listbox.h"

struct chat_window {
  WINDOW* window;
  struct listbox* listbox;
};

struct chat_window* create_chat_window(int width, int height, int start_y, int start_x);

void draw_border(struct chat_window* chat_window);

void add_message(struct chat_window* chat_window, char* sender, char* message);

void move_chat_up(struct chat_window* char_window);

void move_chat_down(struct chat_window* char_window);

void dispose_chat_window(struct chat_window* window);

#endif // !CHAT_WINDOW_H
