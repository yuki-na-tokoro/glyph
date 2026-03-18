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

#include "server.h"
#include "protocol.h"

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

  for (;;) {

    poll(&fd, 1, 0);

    if (fd.revents == POLLIN) {

      struct client *c = accept_client(sockfd);

      while (c != NULL) {

        int bytes_sent = msg_client(c, "pew\n");

        printf("sent %d bytes\n", bytes_sent);

        close(c->fd);
        free(c);
        
        c = accept_client(sockfd);

      }
    }
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
