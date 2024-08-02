#include "../headers/server.h"
#include <complex.h>
#include <signal.h>

struct server* server;

void free_memory();

int main(void) {
  signal(SIGINT, free_memory);
  server = create_server();
  atexit(free_memory);
  run_server(server);
  exit(EXIT_SUCCESS);
}

void free_memory() {
  free_server(server);
  exit(EXIT_SUCCESS);
}
