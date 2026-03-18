#include "networking.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

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

  return sockfd;
}
