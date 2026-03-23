//Befehle für den Server
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#include "commands.h"
#include "client.h"
#include "../logging.h"
#include "../hashtypes.h"
#include "data.h"

#define CMD_COUNT 3

typedef int cmd_fn(struct client*, struct command*);

struct map cmd_fs;

int cap(struct client *c, struct command *cmd);

void init_command_map() {
  cmd_fs = map_init(64);
  map_put(&cmd_fs, "NICK", (void*)nick);
  map_put(&cmd_fs, "CAP", (void*)cap);
}

int raw_msg(struct client *client, char *buffer){
  struct command parsed;

  parse_msg(buffer, &parsed);
   
  cmd_fn* fn = ((cmd_fn*)map_get(&cmd_fs, parsed.command));

  if (fn == NULL) {
    return 0;
  }

  fn(client, &parsed);
  
  return 0;
}

int parse_msg(char *raw, struct command *out) {

  char *tags;
  char *source = NULL;

  char *body;
  
  char *cmd;
  char *params;
  char *trailing;

  if (raw[0] == '@') {
    tags = strtok(raw, " ");
    tracemsg("tags: %s", tags);
    body = strtok(NULL, "");
  } else {
    body = raw; 
  }

  if (body[0] == ':') {
    body++;
    source = strtok(body, " ");
    tracemsg("source: %s", source);
    body = strtok(NULL, "");
  }

  body = strtok(body, ":");
  trailing = strtok(NULL, "");
  cmd = strtok(body, " ");
  tracemsg("cmd: %s", cmd);
  params = strtok(NULL, "");
  tracemsg("params: %s", params);
  tracemsg("trailing: %s", trailing);
 
  out->source = source;
  out->parameters = params;
  out->trailing = trailing;
  out->command = cmd;

  return 0;
}

int nick(struct client *c, struct command *cmd) {
  char *nick = strtok(cmd->parameters, " ");
  c->nick = nick;
  return 0;
}

int user(struct client *c, struct command *cmd) {
  char *username = strtok(cmd->parameters, " ");
  c->username = username;
  c->realname = cmd->trailing;
  return 0;
}

int cap(struct client *c, struct command *cmd) {
  msg_client(c, "CAP * LS");
  return 0;
}
