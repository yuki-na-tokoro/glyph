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
#include "../logging.h"
#include "client.h"

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

  debugmsg("polling listening socket...");
  poll(&fd, 1, TIMEOUT);
  
  if (fd.revents == POLLIN) {

    struct client *c = accept_client(sockfd);

    while (c != NULL) {
      logmsg("trying to accept incoming connection...");

      if (!push_client(clients, MAX_CONN, ccount, c)) {
        errormsg("rejected incoming connection: MAX_CONN reached!");

        close(c->fd);
        free(c);

      } else {

        logmsg("accepted incoming connection.");
      };
      
      c = accept_client(sockfd);

    }
  } else {
    //TODO
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

void poll_clients(struct client* clients[MAX_CONN], int *ccount) {

  struct pollfd fds[*ccount];
  bool closedmask[*ccount];
  bool _close;
  char buffer[128];
  int rc;

  for (int i = 0; i < *ccount; i++) {
    struct pollfd fd = {clients[i]->fd, POLLIN, 0};
    fds[i] = fd;
  }

  debugmsg("polling clients...");
  poll(fds, *ccount, TIMEOUT);
  
  for (int i = 0; i < *ccount; i++) {
    if (fds[i].revents == 0) {
      continue;
    }

    for (;;) {

      if (!(fds[i].revents & POLLIN)) {
        errormsg("#poll_clients(): unexpected revents: %d", fds[i].revents);
        // TODO: exit gracefully
        exit(1);
      }
      
      rc = recv(fds[i].fd, buffer, sizeof(buffer), 0);

      // if nothing is read
      if (rc < 0) {
        if (errno != EWOULDBLOCK) {
          closedmask[i] = true;
        }
        break;
      }

      // if client closed connection
      if (rc == 0) {
        closedmask[i] = true;
        break;
      }

      char *cbuffer = clients[i]->buffer;

      strcat(buffer, cbuffer);
      size_t buflen = strlen(cbuffer);
      if (cbuffer[buflen-1] == '\n' && cbuffer[buflen-2] == '\r') {
        // TODO: send command
        debugmsg("received message: ", cbuffer);
        cbuffer[0] = '\0';
      }
    }

    if (closedmask[i]) _close = 1;
  }

  if (_close) {
    int shift = 0;
    for (int i = 0; i < *ccount; i++) {
      if (closedmask[i]) {
        logmsg("closing client %d...", clients[i]->fd);
        close(clients[i]->fd);
        free(clients[i]);
        shift++;
      } else {
        clients[i-shift] = clients[i];
      }
    }
    *ccount -= shift;
  }
  
}

