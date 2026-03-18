#include <stdbool.h>
#include <string.h>
#include <sys/socket.h>

#include "client.h"

int msg_client(struct client *c, char *msg) {
  return send(c->fd, msg, strlen(msg), 0);
}

bool push_client(struct client* clients[], const int max_conn, int *ccount, struct client *c) {
  if (*ccount >= max_conn) {
    return false;
  }

  clients[*ccount] = c;
  (*ccount)++;
  return true;
}

