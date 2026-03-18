#ifndef CLIENT_H
#define CLIENT_H

struct client {
  int fd;
  struct sockaddr *addr;
};

int msg_client(struct client*, char*);

bool push_client(struct client* clients[], const int max_conn, int* ccount, struct client*); 

#endif // CLIENT_H
