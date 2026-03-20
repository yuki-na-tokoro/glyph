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

  tracemsg("bound socket %d on %s", sockfd, ip4);

  struct client* clients[MAX_CONN]; 
  int ccount = 0;
  bool *mask;

  for (;;) {

    struct pollfd fds[ccount + 1];

    struct pollfd sockpoll = {sockfd, POLLIN, 0};
    fds[0] = sockpoll;

    for (int i = 1; i < ccount + 1; i++) {
      struct pollfd fd = {clients[i-1]->fd, POLLIN, 0};
      fds[i] = fd;
    }

    tracemsg("polling %d sockets (%d clients)", ccount + 1, ccount);

    poll(fds, ccount + 1, TIMEOUT);

    mask = calloc(ccount + 1, sizeof(bool));

    if (!handle_client_poll(fds, clients, &ccount, mask)) {
      break;
    }

    close_clients(mask, clients, &ccount);

    if (!handle_socket_poll(fds[0], clients, &ccount)) {
      break;
    }

    free(mask);
  }

  mask = calloc(ccount + 1, sizeof(bool));
  memset(mask, 1, (ccount + 1) * sizeof(bool));

  close_clients(mask, clients, &ccount);

  free(mask);
  
  close(sockfd);
  freeaddrinfo(res);

}


