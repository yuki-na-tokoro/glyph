#ifndef SERVER_H
#define SERVER_h

#include <sys/socket.h>
#define BACKLOG 10
 
struct client {
  int fd;
  struct sockaddr *addr;
};

struct client* accept_client(int);

#endif // SERVER_H
int msg_client(struct client*, char*);
