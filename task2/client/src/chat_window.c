#include "../headers/chat_window.h"

/**
 * create_chat_window - creates chat window struct object,
 * that is used to display user messages as a list.
 * @username: nickname of user
 * @height: height of chat window
 * @width: width of chat window
 * @start_y: y position from where chat window is drawn
 * @start_x: x position from where chat window is drawn
 *
 * Return: pointer to struct chat_window 
 */ 
struct chat_window* create_chat_window(int height, int width, int start_y, int start_x) {
  struct chat_window* chat_window = (struct chat_window*) malloc(sizeof(struct chat_window));
  
  chat_window->window = newwin(height, width, start_y, start_x);
  chat_window->listbox = create_listbox(height - 2, width - 2, start_y + 1, start_x + 1);
  
  keypad(chat_window->window, TRUE);
  draw_border(chat_window);
  return chat_window;
}
/**
 * process_chat_window_input - used to process user input
 * in chat_window.
 * @chat_window - pointer to chat_window struct object
 *
 * Return: 0 code for program exit, 1 code for window switch  
 */
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

/**
 * draw_border - used to draw border in chat_window.window
 * around contents, and print header of window.
 * @chat_window - pointer to chat_window struct object
 */
void draw_border(struct chat_window* chat_window) {
  box(chat_window->window, 0, 0);
  mvwprintw(chat_window->window, 0, 1, "Messages");
  wrefresh(chat_window->window);
}

/**
 * add_message - used to add message to listbox, formats
 * message as: username message_text.
 * @chat_window - pointer to chat_window struct object
 * @sender - username 
 * @message - message that user send
 */
void add_message(struct chat_window* chat_window, char* sender, char* message) {
  char* buffer = (char*) malloc(ITEM_LENGTH * sizeof(char));
  snprintf(buffer, ITEM_LENGTH, "%s: %s", sender, message);
  
  /* add_item ends with error */
  if (add_item(chat_window->listbox, buffer) != 0) {
    /* delete latest item */
    delete_item(chat_window->listbox, 0);
    add_item(chat_window->listbox, buffer);
  }
  
  free(buffer);
}

/**
 * move_chat_up - used to move listbox up by one item
 * and redraw it.
 * @chat_window - pointer to chat_window struct object
 */
void move_chat_up(struct chat_window* chat_window) {
  move_up(chat_window->listbox);
  update(chat_window->listbox);
}

/**
 * move_chat_down - used to move listbox down by one item
 * and redraw it.
 * @chat_window - pointer to chat_window struct object
 */
void move_chat_down(struct chat_window* chat_window) {
  move_down(chat_window->listbox);
  update(chat_window->listbox);
}

/**
 * dispose_chat_window - used to free memory allocated for
 * chat_window object, deletes window.
 * @chat_window - pointer to chat_window struct object
 */
void dispose_chat_window(struct chat_window* chat_window) {
  dispose(chat_window->listbox);
  delwin(chat_window->window);
  free(chat_window);
}

