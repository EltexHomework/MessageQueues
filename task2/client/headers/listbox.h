#ifndef LISTBOX_H
#define LISTBOX_H
#define ITEM_LENGTH 128 
#define MAX_ITEMS 128

#include <curses.h>
#include <stdlib.h>
#include <string.h>

struct listbox {
  WINDOW *items_window;

  char **items;
  short can_scroll;
  int size;
  int length;
  int width;
  
  // scroll properties, window of items in char** items
  int height;
  int padding_top;
};

struct listbox* create_listbox(int height, int width, int start_y, int start_x);

void update(struct listbox* listbox);

int delete_item(struct listbox* listbox, int index);

void move_up(struct listbox* listbox);

void move_down(struct listbox* listbox);

int add_item(struct listbox* listbox, char* item);

int increase_size(struct listbox* listbox, int amount);

void dispose(struct listbox* listbox);

#endif // !LISTBOX_H
