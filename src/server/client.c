#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "client.h"
#include "../logging.h"
#include "data.h"
#include "networking.h"
#include "commands.h"

int msg_client(struct client *c, char *fmt, ...) {
  char *msg;
  va_list args;
  va_start(args, fmt);
  if (vasprintf(&msg, fmt, args) < 0) {
    errormsg("#msg_client(): vasprintf error");
    va_end(args);
    return -1;
  }

  va_end(args);
  int ret = send(c->fd, msg, strlen(msg), 0);
  free(msg);
  return ret;
}

bool push_client(struct client* clients[], int *ccount, struct client *c) {
  if (*ccount >= MAX_CONN) {
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
        tracemsg("closing socket %d", clients[i]->fd);
        close_client(clients[i]);
        shift++;
      } else {
        clients[i-shift] = clients[i];
      }
    }

    *ccount -= shift;
  }
}

void close_client(struct client *client) {
  for (int i = 0; i < client->channels.cap; i++) {
    struct channel *channel = client->channels.buckets[i].value;
    if (channel != NULL) {
      map_remove(&channel->members, client->username);
      if (channel->members.size == 0) {
        map_remove(get_channel_map(), channel->name);
        free(channel);
      } else {
        set_remove(&channel->op, client->username);
      }
    }
  }
  close(client->fd);
  free(client);
}
