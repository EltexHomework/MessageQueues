#include "../headers/message.h"

char* toString(const struct message message) {
  char* buffer = (char*) malloc(MESSAGE_LEN * sizeof(char));
  snprintf(buffer, MESSAGE_LEN, "%s: %s", message.username, message.text);
  return buffer;
}
