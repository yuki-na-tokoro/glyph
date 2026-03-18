#ifndef SERVER_H
#define SERVER_h

#include <sys/socket.h>
#define BACKLOG 10
 
struct client {
  int sockfd;
  struct sockaddr *addr;
};

struct client* accept_client(int);

#endif // SERVER_H