#ifndef SERVER_H
#define SERVER_H

#define BACKLOG 10
#define MAX_CONN 128
 
struct client {
  int fd;
  struct sockaddr *addr;
};

struct client* accept_client(int fd);

int msg_client(struct client*, char*);

void poll_listening(int, struct client* clients[MAX_CONN], int* ccount);

bool push_client(struct client* clients[MAX_CONN], int* ccount, struct client*); 

#endif // SERVER_H
