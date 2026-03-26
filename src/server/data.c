#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data.h"
#include "../logging.h"
#include "client.h"

struct map chn_map;

void init_channel_map(void) {
  chn_map = map_init(64);
}

struct map* get_channel_map(void) {
  return &chn_map;
}

void create_channel(char *name, struct client *op) {
  debugmsg("#create_channel(): creating channel %s", name);
  struct channel *channel;

  if ((channel = malloc(sizeof(struct channel))) == NULL)  {
    // TODO: error handling
  }

  strcpy(channel->name, name);

  channel->op = set_init(4);
  channel->members = map_init(16);

  set_put(&channel->op, op->username);

  map_put(&channel->members, op->username, op);
  map_put(&chn_map, name, channel);
  map_put(&op->channels, name, channel);    
}

void delete_channel(struct channel *channel) {
  debugmsg("#delete_channel(): deleting channel %s", channel->name);
  map_remove(&chn_map, channel->name);
  free(channel);
}

void leave_channel(struct channel *channel, struct client *client) {
  debugmsg("#leave_channel(): client %s left channel %s", client->username, channel->name);

  for (size_t i = 0; i < channel->members.cap; i++){
    struct client *member = channel->members.buckets[i].value;

    if(member != NULL){
      msg_client(client,"Server: %-9s verleasst den Channel !");
    }
  }
  
  map_remove(&channel->members, client->username);
  if (channel->members.size == 0) {
    delete_channel(channel);
  } else {
    set_remove(&channel->op, client->username);
  }
}
