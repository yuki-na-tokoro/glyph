#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "client.h"
#include "../logging.h"

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

void close_clients(bool *mask, struct client* clients[], int* ccount) {
  
  if (mask[0]) {
    int shift = 0;
    for (int i = 0; i < *ccount; i++) {
      if (mask[i+1]) {
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


