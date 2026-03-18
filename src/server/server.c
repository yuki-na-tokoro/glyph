#include <errno.h>
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
#include "server.h"
#include "../protocol.h"

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

  printf("bound on: %s\n", ip4);

  // set socket to listening
  if (listen(sockfd, BACKLOG) == -1) {
    perror("listen error");
    exit(1);
  }

  struct pollfd fd;

  fd.fd = sockfd;
  fd.events = POLLIN;

  struct client* clients[MAX_CONN]; 
  int ccount = 0;

  for (;;) {

    poll_listening(sockfd, clients, &ccount);

  }
  
  close(sockfd);
  freeaddrinfo(res);
  
}

struct client* accept_client(int sockfd) {
  struct sockaddr_storage client_addr;

  socklen_t addr_size = sizeof client_addr;
  int fd = accept(sockfd, (struct sockaddr *) &client_addr, &addr_size);

  if (fd == -1) {
    if (errno != EWOULDBLOCK) {
      perror("accept error");
      exit(1);
    }
    return NULL;
  }

  struct client *c = malloc(sizeof(struct client));

  c->addr = (struct sockaddr *) &client_addr;
  c->fd = fd;

  return c;
}

int msg_client(struct client *c, char *msg) {
  return send(c->fd, msg, strlen(msg), 0);
}

void poll_listening(int sockfd, struct client* clients[MAX_CONN], int *ccount) {
   
  struct pollfd fd = {sockfd, POLLIN, 0};

  poll(&fd, 1, 0);
  
  if (fd.revents == POLLIN) {

    struct client *c = accept_client(sockfd);

    while (c != NULL) {

      if (!push_client(clients, ccount, c)) {
        fprintf(stderr, "rejecting incoming connection: MAX_CONN reached");

        close(c->fd);
        free(c);
      };
      
      c = accept_client(sockfd);

    }
  }
}

bool push_client(struct client* clients[MAX_CONN], int *ccount, struct client *c) {
  if (*ccount >= MAX_CONN) {
    return false;
  }

  clients[*ccount] = c;
  (*ccount)++;
  return true;
}
