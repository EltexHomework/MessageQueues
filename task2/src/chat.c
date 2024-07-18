#include "../headers/chat.h"

struct chat* create_chat() {
  struct chat* chat = (struct chat*) malloc(sizeof(struct chat));
  chat->chat_window = create_chat_window(50, 160, 0, 0);
  chat->users_window = create_users_window(50, 30, 0, 160);
  chat->current_window = 1;

  for (int i = 1; i < MAX_ITEMS + 128; i++) {
    char* sender = (char*) malloc(128 * sizeof(char));
    snprintf(sender, sizeof(sender), "%d", i);
    add_message(chat->chat_window, sender, "asdasdasdasd");
    free(sender);
  }
  
  for (int i = 1; i < MAX_ITEMS + 128; i++) {
    char* sender = (char*) malloc(128 * sizeof(char));
    snprintf(sender, sizeof(sender), "%d", i);
    add_user(chat->users_window, sender);
    free(sender);
  }

  update(chat->users_window->listbox);
  update(chat->chat_window->listbox);

  return chat;
}

void run_chat(struct chat* chat) {
  while (1) {
    // chat
    if (chat->current_window == 1) {
      if (process_chat_window_input(chat->chat_window) == 1) {
        chat->current_window++; 
      } else {
        break;
      }
    }
    // users
    else if (chat->current_window == 2) {
      if (process_users_window_input(chat->users_window) == 1) {
        chat->current_window++; 
      } else {
        break;
      }
    } 
    // input_field
    else {
      chat->current_window = 1;
    }
  }

  dispose_chat(chat);
  endwin();
}

void dispose_chat(struct chat* chat) {
  dispose_chat_window(chat->chat_window);
  dispose_users_window(chat->users_window);
  free(chat);
}

