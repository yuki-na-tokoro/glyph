#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
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

  if ((status = getaddrinfo(NULL, PORT, &hints, &res)) != 0) {
    fprintf(stderr, "gai error: %s\n", gai_strerror(status));
    exit(1);
  };

  sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
  if (sockfd == -1) {
    fprintf(stderr, "socket error\n");
    exit(1);
  }

  if (bind(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
    perror("bind error");
    exit(1);
  }

  char ip4[INET_ADDRSTRLEN];
  struct sockaddr_in *sa = (struct sockaddr_in*) res->ai_addr;

  inet_ntop(AF_INET, &(sa->sin_addr), ip4, INET_ADDRSTRLEN);

  printf("bound on: %s\n", ip4);

  if (listen(sockfd, BACKLOG) == -1) {
    perror("listen error");
    exit(1);
  }

  while (1) {

    struct client *c = accept_client(sockfd);

    char *msg = "pew\n";
    int len = strlen(msg);

    int bytes_sent = send(c->sockfd, msg, len, 0);

    close(c->sockfd);
    free(c);
    
  }
  
  close(sockfd);
  freeaddrinfo(res);
  
}

struct client* accept_client(int sockfd) {
  struct sockaddr_storage client_addr;

  socklen_t addr_size = sizeof client_addr;
  int client_fd = accept(sockfd, (struct sockaddr *) &client_addr, &addr_size);
  if (client_fd == -1) {
    perror("accept error\n");
    exit(1);
  }

  struct client *c = malloc(sizeof(struct client));

  c->addr = (struct sockaddr *) &client_addr;
  c->sockfd = client_fd;

  return c;
}

