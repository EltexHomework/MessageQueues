#ifndef CONNECTION_REQUEST_H
#define CONNECTION_REQUEST_H
#define USERNAME_LEN 128

enum connection_type { CONNECT, DISCONNECT };

struct connection_request {
  char username[USERNAME_LEN];
  enum connection_type type;   
};

#endif // !CONNECTION_REQUEST_H
