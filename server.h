
#include <sys/socket.h>
#define BACKLOG 10
 
struct client {
  int sockfd;
  struct sockaddr *addr;
};

struct client* accept_client(int);
