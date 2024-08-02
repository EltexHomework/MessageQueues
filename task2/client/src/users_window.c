#include "../headers/users_window.h"

/**
 * create_users_window - used to create an object
 * of uses_window.
 * @height - height of users_window 
 * @width - width of users_window 
 * @start_y - y position where users window is drawn from
 * @start_x - x position where users window is drawn from
 *
 * Return: pointer to users_window object
 */
struct users_window* create_users_window(int height, int width, int start_y, int start_x) {
  struct users_window* users_window = (struct users_window*) malloc(sizeof(struct users_window));
  
  users_window->window = newwin(height, width, start_y, start_x);
  users_window->listbox = create_listbox(height - 2, width - 2, start_y + 1, start_x + 1);
  
  keypad(users_window->window, TRUE);
  draw_users_border(users_window);
  return users_window;
}

/**
 * process_users_window_input - used to get users users input
 * on users window.
 * @users_window - pointer to object of users_window struct
 *
 * Return: 0 on exit, 1 on Tab key  
 */
int process_users_window_input(struct users_window* users_window) {
  int c;
  while ((c = wgetch(users_window->window)) != KEY_F(1)) {
    switch (c) {
      case KEY_DOWN:
        move_users_down(users_window);
        break;
      case KEY_UP:
        move_users_up(users_window);
        break;
      /* Tab key */
      case 9:
        return 1;
    }
  }

  return 0;
}

/**
 * draw_users_border - used to draw border around window contents
 * and header of window.
 * @users_window - pointer to an object of users_window struct
 */
void draw_users_border(struct users_window* users_window) {
  box(users_window->window, 0, 0);
  mvwprintw(users_window->window, 0, 1, "Users");
  wrefresh(users_window->window);
}

/**
 * add_user - used to add user to listbox. If overflow of
 * listbox occurs, increments its size by 1.
 * @users_window - pointer to an object of users_window struct 
 * @user - username of user 
 */
void add_user(struct users_window* users_window, char* user) {
  if (add_item(users_window->listbox, user) != 0) {
    increase_size(users_window->listbox, 1); 
    add_item(users_window->listbox, user);
  }
}

/**
 * delete_user - used to delete user from listbox. Finds
 * first occurance of username in listbox, deletes item
 * and exits.
 * @users_window - pointer to an object of users_window struct
 * @username - username of user to delete
 */
void delete_user(struct users_window* users_window, char* username) {
  for (int i = 0; i < users_window->listbox->size; i++) {
    if (strcmp(username, users_window->listbox->items[i]) == 0) {
      delete_item(users_window->listbox, i);
      break;
    }
  }
}

/**
 * move_users_up - used to scroll users window up by one
 * item.
 * @users_window - pointer to an object of users_window struct
 */
void move_users_up(struct users_window* users_window) {
  move_up(users_window->listbox);
  update(users_window->listbox);
}

/**
 * move_users_down - used to scroll users window down by one
 * item.
 * @users_window - pointer to an object of users_window struct
 */
void move_users_down(struct users_window* users_window) {
  move_down(users_window->listbox);
  update(users_window->listbox);
}

/**
 * dispose_users_window - frees memory that is allocated for
 * users_window object.
 * @users_window - pointer to an object of users_window struct
 */
void dispose_users_window(struct users_window* users_window) {
  dispose(users_window->listbox);
  delwin(users_window->window);
  free(users_window);
}


