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
    errormsg("socket error");
    exit(1);
  }

  tracemsg("got socket with id %d", sockfd);

  const int enable = 1;

  // set socket to reuse address and port
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &enable, sizeof(int)) == -1) {
    errormsg("setsockopt error");
    exit(1);
  }

  if(!set_nonblocking(sockfd)) {
    exit(1);
  }

  // bind socket to address
  if (bind(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
    errormsg("bind error");
    exit(1);
  }

  // set socket to listening
  if (listen(sockfd, BACKLOG) == -1) {
    errormsg("listen error");
    exit(1);
  }

  return sockfd;
}

bool set_nonblocking(int sockfd) {

  int flags = fcntl(sockfd, F_GETFL, 0);
  if (flags == -1) {
    errormsg("fcntl F_GETFL error");
    return false;
  }

  flags = flags | O_NONBLOCK;
  if (fcntl(sockfd, F_SETFL, flags) == -1) {
    errormsg("fcntl F_SETFL error");
    return false;
  }
  tracemsg("set socket %d to non-blocking", sockfd);
  return true;
}

int accept_client(int sockfd, struct client* c) {
  struct sockaddr_storage client_addr;

  socklen_t addr_size = sizeof client_addr;
  int fd = accept(sockfd, (struct sockaddr *) &client_addr, &addr_size);

  if (fd == -1) {
    if (errno != EWOULDBLOCK) {
      errormsg("accept error");
      return -1;
    }
    return 0;
  }

  if (!set_nonblocking(fd)) {
    return -1;
  };

  c->addr = (struct sockaddr *) &client_addr;
  c->fd = fd;
  c->buffer = calloc(MSG_SIZE, sizeof(char));
  if (c->buffer == NULL) {
    errormsg("#accept_client(): calloc error");
    return -1;
  }

  return 1;
}

bool handle_client_poll(struct pollfd fds[], struct client *clients[MAX_CONN], int *ccount, bool *mask) {

  int rc;
  
  char buffer[MSG_SIZE];
  
  for (int i = 1; i < *ccount + 1; i++) {

    if (fds[i].revents == 0) {
      tracemsg("no events on socket %d", fds[i].fd);
      continue;
    }

    for (;;) {

      if (!(fds[i].revents & POLLIN)) {
        errormsg("#poll_clients(): unexpected revents: %d", fds[i].revents);
        return false;
      }

      tracemsg("polling returned on socket %d", fds[i].fd);
      
      rc = recv(fds[i].fd, buffer, sizeof(buffer), 0);

      if (rc > 0) {
        tracemsg("read %d bytes from socket %d", rc, fds[i].fd);
      }

      // if nothing is read
      if (rc < 0) {
        if (errno != EWOULDBLOCK) {
          tracemsg("socket %d disconnected", fds[i].fd);
          mask[i] = true;
        }
        break;
      }

      // if client closed connection
      if (rc == 0) {
        mask[i] = true;
        break;
      }

      strcat(clients[i-1]->buffer, buffer);
      
      size_t buflen = strlen(clients[i-1]->buffer);

      if (buflen < 2) break;

      char* ret;
      uint msglen;

      while ((ret = strstr(clients[i-1]->buffer, "\r\n")) != NULL) {
        msglen = strlen(clients[i-1]->buffer) - strlen(ret);

        memcpy(buffer, clients[i-1]->buffer, msglen);
        buffer[msglen] = '\0';

        tracemsg("received message: %s (%d bytes)", buffer, msglen);

        raw_msg(clients[i-1], buffer);

        clients[i-1]->buffer = ret + 2;
      }
    }

    if (mask[i]) mask[0] = 1;
  }

  return true;
}

bool handle_socket_poll(struct pollfd spollfd, struct client *clients[MAX_CONN], int *ccount) {

  if (spollfd.revents == 0) return true;
  
  tracemsg("polling returned on listening");

  if (!(spollfd.revents & POLLIN)) {
    errormsg("#poll_clients(): unexpected revents: %d", spollfd.revents);
    return false;
  }

  struct client *c = malloc(sizeof(struct client));
  int rc = accept_client(spollfd.fd, c);
  
  if (rc == -1) {
    return false;
  };

  while (rc != 0) {
    tracemsg("trying to accept socket %d", c->fd);

    if (!push_client(clients, ccount, c)) {
      warnmsg("rejected incoming connection: MAX_CONN reached!");

      close(c->fd);
      free(c);
      return true;

    } else {
      tracemsg("accepted socket %d", c->fd);
    };

    if ((rc = accept_client(spollfd.fd, c)) == -1) {
      return false;
    }

  }

  return true;
}
