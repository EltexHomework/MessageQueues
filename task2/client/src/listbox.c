#include "../headers/listbox.h"

/**
 * create_listbox - used to create an object of struct listbox.
 * Initializes items contents as NULL items.
 * @height - height of listbox
 * @width - width of listbox
 * @start_y - y position where listbox is drawn from
 * @start_x - x position where listbox is drawn from
 *
 * Return: pointer to listbox struct
 */
struct listbox* create_listbox(int height, int width, int start_y, int start_x) {
  struct listbox* listbox = (struct listbox*) malloc(sizeof(struct listbox));
  
  listbox->width = width;
  listbox->height = height;
  listbox->padding_top = 0;
  listbox->can_scroll = 0;
  listbox->length = 0;
  listbox->items_window = newwin(height, width, start_y, start_x);
  listbox->size = MAX_ITEMS;  
  listbox->items = (char **) malloc(listbox->size * sizeof(char*));
  for (int i = 0; i < listbox->size; i++) {
    listbox->items[i] = NULL;
  }

  return listbox;
}

/**
 * update - is used to redraw listbox contents.
 * @listbox - pointer to listbox struct object
 */
void update(struct listbox* listbox) {
  int x, y;
  int max_elements = listbox->height * 2;
  int start = listbox->padding_top;
  int end = (start + max_elements < listbox->length) ? start + max_elements : listbox->length;   

  wclear(listbox->items_window);
  getyx(listbox->items_window, y, x);
  
  for (int i = listbox->padding_top; i < end; i++) {
    int item_height = strlen(listbox->items[i]) / (listbox->width * 2);
    mvwprintw(listbox->items_window, x, y, "%s", listbox->items[i]);
    
    x += 1 + item_height;
  }

  wrefresh(listbox->items_window);
}

/**
 * delete_item - used to delete an item in listbox
 * on index position. Frees memory that allocated for
 * item. Moves item pointers from index + 1 to left,
 * decrements size.
 * @listbox - pointer to listbox struct object
 * @index - index of item to delete
 *
 * Return: 0 if successful, 1 if trying to delete NULL item  
 */
int delete_item(struct listbox* listbox, int index) {
  if (listbox->items[index] == NULL) {
    return 1; 
  }
  
  free(listbox->items[index]);
  listbox->items[index] = NULL;

  // move items to left by 1 
  int i = index + 1;
  while (listbox->items[i] != NULL || i != listbox->length)  {
    listbox->items[i - 1] = listbox->items[i];
    i++;
  }           
  listbox->items[i] = NULL; 
  listbox->length--;

  return 0;
}

/**
 * move_up - used to scroll listbox up by one item.
 * @listbox - pointer to listbox struct object 
 */ 
void move_up(struct listbox* listbox) {
  if (!listbox->can_scroll) {
    return;
  }
  
  if (listbox->padding_top == 0) {
    return;  
  }

  listbox->padding_top--;
}

/**
 * move_down - used to scroll listbox down by one item.
 * @listbox - pointer to listbox struct object 
 */ 
void move_down(struct listbox* listbox) {
  if (!listbox->can_scroll) {
    return;
  }
  
  if (listbox->padding_top == listbox->length - 1) {
    return;
  }

  listbox->padding_top++;
}

/**
 * add_item - used to append item in end of listbox.
 * @listbox - pointer to listbox struct object
 * @item - string to add
 *
 * Return: 0 on successful, 1 on arr overflow
 */
int add_item(struct listbox* listbox, char* item) {
  if (listbox->length == listbox->size) {
    return 1; 
  }

  char* item_copy = strndup(item, sizeof(char) * ITEM_LENGTH);

  listbox->items[listbox->length] = item_copy;
  listbox->length++;
  
  if (listbox->length > (listbox->height / 2)) {
    listbox->can_scroll = 1;
  }
  return 0;
}

/**
 * increase_size - increases collection size for amount
 * of items.
 * @listbox - pointer to listbox struct object
 * @amount - amount of items to increase size on
 *
 * Return: 0 on successful
 */
int increase_size(struct listbox* listbox, int amount) {
  listbox->size += amount;

  listbox->items = (char**) realloc(listbox->items, listbox->size * sizeof(char*)); 
  for (int i = listbox->length; i < listbox->size; i++) {
    listbox->items[i] = NULL;
  }
  
  return 0;
}

/**
 * dispose - used to free memory allocated for listbox,
 * frees items in collection, collection, window, and itself.
 * @listbox - pointere to listbox struct object
 */
void dispose(struct listbox* listbox) {
  for (int i = 0; i < listbox->size; i++) {
    free(listbox->items[i]);
  }
  free(listbox->items);
  delwin(listbox->items_window);
  free(listbox); 
}

