#include "../headers/input_field.h"
#include <curses.h>

struct input_field* create_field(int height, int width, int start_y, int start_x) {
  struct input_field* field = (struct input_field*) malloc(sizeof(struct input_field));
  field->window = newwin(height, width, start_y, start_x);
  field->height = height;
  field->width = width;
  field->offset = 0;
  clear_str(field);
  keypad(field->window, TRUE);
  update_field(field);
  return field;
}

void update_field(struct input_field* field) {
  wclear(field->window);
  box(field->window, 0, 0);
  int len = strlen(field->str);
  int start_pos = field->offset * (field->width - 2); 

  for (int i = 0; i < field->height - 2; ++i) {
    if (start_pos + i * (field->width - 2) < len) {
      mvwprintw(field->window, 1 + i, 1, "%.*s", field->width - 2, field->str + start_pos + i * (field->width - 2));
    }
  }
  wrefresh(field->window);
}

void move_field_down(struct input_field* field) {
  int len = strlen(field->str);
  if ((field->offset + 1) * (field->width - 2) < len) {
    field->offset++;
  }
  update_field(field);
}

void move_field_up(struct input_field* field) {
  if (field->offset == 0) {
    return;
  }
  field->offset--;
}

int process_field_input(struct input_field* field) {
  int c;
  while ((c = wgetch(field->window)) != KEY_F(1)) {
    switch (c) {
      case KEY_DOWN:
        move_field_down(field);
        break;
      case KEY_UP:
        move_field_up(field);
        break;
      // Tab
      case 9:
        return 1;
      // Enter
      case 10:
        return 2;
        break;
      case KEY_BACKSPACE:
        if (strlen(field->str) > 0) {
          field->str[strlen(field->str) - 1] = '\0';
        }
        break;
      default:
        if (isprint(c))
          add_char(field, c);
    }
    update_field(field);
  }
  return 0;
}

void add_char(struct input_field* field, char c) {
  int len = strlen(field->str);
  if (len < STR_SIZE - 1) {
      field->str[len] = c;
      field->str[len + 1] = '\0';
  }
}

char* get_str(struct input_field* field) {
  char* str = (char*) malloc(STR_SIZE * sizeof(char));
  strncpy(str, field->str, STR_SIZE);
  clear_str(field);
  return str;
}

void clear_str(struct input_field* field) {
  memset(field->str, 0, STR_SIZE);
  field->offset = 0;
  wrefresh(field->window);
}

void dispose_field(struct input_field* field) {
  delwin(field->window);
  free(field);
}
