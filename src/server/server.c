#include <fcntl.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdbool.h>

#include "networking.h"
#include "client.h"
#include "../protocol.h"
#include "../logging.h"

int main(void) {

  int status;
  struct addrinfo hints, *res;
  int sockfd;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  // get local addrinfo to use
  if ((status = getaddrinfo(NULL, PORT, &hints, &res)) != 0) {
    fprintf(stderr, "gai error: %s\n", gai_strerror(status));
    exit(1);
  };

  sockfd = init_socket(res);
  
  char ip4[INET_ADDRSTRLEN];
  struct sockaddr_in *sa = (struct sockaddr_in*) res->ai_addr;

  inet_ntop(AF_INET, &(sa->sin_addr), ip4, INET_ADDRSTRLEN);

  logmsg("bound on: %s", ip4);

  struct client* clients[MAX_CONN]; 
  int ccount = 0;

  for (;;) {

    poll_listening(sockfd, clients, &ccount);

  }
  
  close(sockfd);
  freeaddrinfo(res);
  
}


