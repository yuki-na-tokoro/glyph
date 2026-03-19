#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdbool.h>

#include "networking.h"

int init_socket(struct addrinfo* res) {

  int sockfd;
  
  // create socket
  sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
  if (sockfd == -1) {
    fprintf(stderr, "socket error\n");
    exit(1);
  }

  const int enable = 1;

  // set socket to reuse address and port
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &enable, sizeof(int)) == -1) {
    perror("setsockopt error");
    exit(1);
  }

  int flags = fcntl(sockfd, F_GETFL, 0);
  if (flags == -1) {
    perror("fcntl F_GETFL error");
    exit(1);
  }
  flags = flags | O_NONBLOCK;
  if (fcntl(sockfd, F_SETFL, flags) == -1) {
    perror("fcntl F_SETFL error");
    exit(1);
  }

  // bind socket to address
  if (bind(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
    perror("bind error");
    exit(1);
  }

  // set socket to listening
  if (listen(sockfd, BACKLOG) == -1) {
    perror("listen error");
    exit(1);
  }

  return sockfd;
}

void poll_listening(int sockfd, struct client* clients[MAX_CONN], int *ccount) {
   

  struct pollfd fd = {sockfd, POLLIN, 0};

  poll(&fd, 1, TIMEOUT);
  
  if (fd.revents == POLLIN) {

    struct client *c = accept_client(sockfd);
    printf("trying client");

    while (c != NULL) {

      if (!push_client(clients, MAX_CONN, ccount, c)) {
        fprintf(stderr, "rejecting incoming connection: MAX_CONN reached");

        close(c->fd);
        free(c);

      } else {

        printf("accepted client");

      };
      
      c = accept_client(sockfd);
      printf("trying client");

    }
  }
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

