#include "../headers/server.h"
#include <signal.h>

struct server* server;

void free_memory();

int main(void) {
  server = create_server();
  //signal(SIGINT, free_memory);
  run_server(server); 
  return 0;  
}

void free_memory() {
  free_server(server); 
}
