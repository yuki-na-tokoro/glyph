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
#include "commands.h"


// TODO: Add extra Data to Client struct
        
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

  set_nonblocking(sockfd);

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

void set_nonblocking(int sockfd) {

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

  set_nonblocking(fd);

  struct client *c = malloc(sizeof(struct client));
  if (c == NULL) {
    perror("#accept_client(): malloc error");
    // TODO: exit gracefully
    exit(1);
  }

  c->addr = (struct sockaddr *) &client_addr;
  c->fd = fd;
  c->buffer = calloc(MSG_SIZE, sizeof(char));
  if (c->buffer == NULL) {
    perror("#accept_client(): calloc error");
    // TODO: exit gracefully
    exit(1);
  }

  return c;
}

bool* handle_client_poll(struct pollfd fds[], struct client *clients[MAX_CONN], int *ccount) {

  int rc;
  bool *mask = calloc(*ccount + 1, sizeof(bool));
  if (mask == NULL) {
    perror("#handle_client_poll(): calloc error");
    // TODO: exit gracefully
    exit(1);
  }
  char buffer[MSG_SIZE];
  
  for (int i = 1; i < *ccount + 1; i++) {

    if (fds[i].revents == 0) {
      continue;
    }

    debugmsg("polling returned on client %d", fds[i].fd);

    for (;;) {

      if (!(fds[i].revents & POLLIN)) {
        errormsg("#poll_clients(): unexpected revents: %d", fds[i].revents);
        // TODO: exit gracefully
        exit(1);
      }
      
      rc = recv(fds[i].fd, buffer, sizeof(buffer), 0);
      debugmsg("read %d bytes from client %d", rc, fds[i].fd);

      // if nothing is read
      if (rc < 0) {
        if (errno != EWOULDBLOCK) {
          mask[i] = true;
        }
        break;
      }

      // if client closed connection
      if (rc == 0) {
        mask[i] = true;
        break;
      }

      char *cbuffer = clients[i-1]->buffer;

      strcat(cbuffer, buffer);

      debugmsg("client buffer is now: %s", cbuffer);
      
      size_t buflen = strlen(cbuffer);

      if (buflen < 2) break;

      if (cbuffer[buflen-1] == '\n' && cbuffer[buflen-2] == '\r') {
        // cbuffer[buflen-2] = '\0';

        debugmsg("received message: %s", cbuffer);

        raw_msg(clients[i-1], cbuffer);

        cbuffer[0] = '\0';
      }
    }

    if (mask[i]) mask[0] = 1;
  }
  return mask;
}

void handle_socket_poll(struct pollfd spollfd, struct client *clients[MAX_CONN], int *ccount) {

  if (spollfd.revents == 0) return;
  
    debugmsg("polling returned on listening");
  
    if (!(spollfd.revents & POLLIN)) {
      errormsg("#poll_clients(): unexpected revents: %d", spollfd.revents);
      // TODO: exit gracefully
      exit(1);
    }

    struct client *c = accept_client(spollfd.fd);

    while (c != NULL) {
      logmsg("trying to accept incoming connection...");

      if (!push_client(clients, ccount, c)) {
        errormsg("rejected incoming connection: MAX_CONN reached!");

        close(c->fd);
        free(c);

      } else {
        logmsg("accepted incoming connection.");
      };
  
      c = accept_client(spollfd.fd);

    }
  
}
