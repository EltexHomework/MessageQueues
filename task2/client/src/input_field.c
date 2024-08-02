#include "../headers/input_field.h"
#include <curses.h>

/**
 * create_field - used to create object of input_field struct.
 * @height - height of input field 
 * @width - width of input field 
 * @start_y - y position where field is drawn from 
 * @start_x - x position where field is drawn from
 * 
 * Return: pointer to input_field object 
 */
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

/**
 * update_field - is used to update redraw contents of field.
 * Considers width of window to correctly wrap contents.
 * @field - pointer to input_field struct object
 */
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

/**
 * move_field_down - used to scroll down input_field 
 * by one item.
 * @field - pointer to input_field struct object
 */
void move_field_down(struct input_field* field) {
  int len = strlen(field->str);
  if ((field->offset + 1) * (field->width - 2) < len) {
    field->offset++;
  }
  update_field(field);
}

/**
 * move_field_up - used to scroll up input_field by
 * one item.
 * @field - pointer to input_field struct object
 */
void move_field_up(struct input_field* field) {
  if (field->offset == 0) {
    return;
  }
  field->offset--;
}

/**
 * process_field_input - used to receive user input
 * and analyze it.
 * @field - pointer to input_field struct object
 *
 * Return: code 1 for tab key, code 2 for enter key,
 * code 0 for exit
 */
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
      /* Tab key */
      case 9:
        return 1;
      /* Enter key */
      case 10:
        return 2;
        break;
      case KEY_BACKSPACE:
        if (strlen(field->str) > 0) {
          field->str[strlen(field->str) - 1] = '\0';
        }
        break;
      /* printable chars */
      default:
        if (isprint(c))
          add_char(field, c);
    }
    update_field(field);
  }
  return 0;
}

/**
 * add_char - used to add character to end of string.
 * Checks for string overflow.
 * @field - pointer to input_field struct object
 * @c - character to add in the end of str
 */ 
void add_char(struct input_field* field, char c) {
  int len = strlen(field->str);
  if (len < STR_SIZE - 1) {
      field->str[len] = c;
      field->str[len + 1] = '\0';
  }
}

/**
 * get_str - used to get string from input field.
 * Clears string contents. Malloc's buffer for string,
 * should be freed when is not used anymore.
 * @field - pointer to input_field struct object
 *
 * Return: pointer to a string length of STR_SIZE
 * specified in config.h
 */
char* get_str(struct input_field* field) {
  char* str = (char*) malloc(STR_SIZE * sizeof(char));
  strncpy(str, field->str, STR_SIZE);
  clear_str(field);
  return str;
}

/**
 * clear_str - used to clear string contents. Calls
 * update_field to redraw field contents.
 * @field - pointer to input_field struct object 
 */
void clear_str(struct input_field* field) {
  memset(field->str, 0, STR_SIZE);
  field->offset = 0;
  update_field(field);
}

/**
 * dispose_field - used to free allocated memory for
 * input_field, delete window.
 * @field - pointer to input_field struct object
 */
void dispose_field(struct input_field* field) {
  delwin(field->window);
  free(field);
}
