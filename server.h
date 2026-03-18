#ifndef SERVER_H
#define SERVER_H

#include <sys/socket.h>
#define BACKLOG 10
 
struct client {
  int fd;
  struct sockaddr *addr;
};

struct client* accept_client(int);

int msg_client(struct client*, char*);
#endif // SERVER_H
