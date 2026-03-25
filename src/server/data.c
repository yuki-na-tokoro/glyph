
#include "data.h"
#include <stdlib.h>
#include <string.h>

struct map chn_map;

void init_channel_map(void) {
  chn_map = map_init(64);
}

struct map* get_channel_map(void) {
  return &chn_map;
}

void create_channel(char *name, struct client *op) {
  struct channel *channel;

  if ((channel = malloc(sizeof(struct channel))) == NULL)  {
    // TODO: error handling
  }

  strcpy(channel->name, name);

  channel->op = set_init(4);
  channel->members = map_init(16);

  set_put(&channel->op, op->username);

  map_put(&channel->members, op->username, op);
  map_put(&chn_map, name, &channel);
  map_put(&op->channels, name, &channel);    
}

void delete_channel(struct channel *channel) {
  map_remove(&chn_map, channel->name);
  free(channel);
}

void leave_channel(struct channel *channel, struct client* client) {
  map_remove(&channel->members, client->username);
  if (channel->members.size == 0) {
    delete_channel(channel);
  } else {
    set_remove(&channel->op, client->username);
  }
}
