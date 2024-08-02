#ifndef USERS_WINDOW_H
#define USERS_WINDOW_H

#include <curses.h>
#include <stdio.h>
#include "listbox.h"

/* Users windows add field related logic to blank
 * listbox.
 *
 * [Key bindings]
 * 
 * - Tab - used to change focus on windows
 * - Key down - used on all windows to scroll down
 * - Key up - used on all windows to scroll up
 * - F1 - used to exit program
 * 
 * [Example]
 *  
 */
struct users_window {
  WINDOW* window;
  struct listbox* listbox;
};

struct users_window* create_users_window(int width, int height, int start_y, int start_x);

int process_users_window_input(struct users_window* users_window);

void draw_users_border(struct users_window* users_window);

void add_user(struct users_window* users_window, char* username);

void delete_user(struct users_window* users_window, char* username);

void move_users_up(struct users_window* users_window);

void move_users_down(struct users_window* users_window);

void dispose_users_window(struct users_window* window);

#endif // USERS_WINDOW_H
