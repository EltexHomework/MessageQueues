#ifndef INPUT_FIELD_H
#define INPUT_FIELD_H
#define STR_SIZE 128 

#include <curses.h>
#include <stdlib.h>

struct input_field {
  WINDOW* window;
  char str[STR_SIZE]; 
};

struct input_field* create_field(int width, int height, int start_y, int start_x);

void update_field(struct input_field* field);

int process_field_input(struct input_field* field);

void add_char(struct input_field* field, char c); 

void clear_str(struct input_field* field);

void dispose_field(struct input_field* field);

#endif // !INPUT_FIELD_H

