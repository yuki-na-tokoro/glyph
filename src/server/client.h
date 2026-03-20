#ifndef CLIENT_H
#define CLIENT_H

struct client {
  int fd;
  struct sockaddr *addr;
  char* buffer;
};

int msg_client(struct client*, char*);

bool push_client(struct client* clients[], const int max_conn, int* ccount, struct client*); 

void close_clients(bool *mask, struct client* clients[], int* ccount);

#endif // CLIENT_H
