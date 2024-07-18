#include "../headers/input_field.h"

struct input_field* create_field(int height, int width, int start_y, int start_x) {
  struct input_field* field = (struct input_field*) malloc(sizeof(struct input_field));
  
  field->window = newwin(height, width, start_y, start_x);

  return field;
}

int process_field_input(struct input_field* field) {
     
}

void add_char(struct input_field* field, char c) {
    
}

void clear_str(struct input_field* field) {
     
}

void dispose_field(struct input_field* field) {
  delwin(field->window);
  free(field);
}

