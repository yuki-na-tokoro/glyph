//Befehle für den Server
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#include "commands.h"
#include "../logging.h"
#include "../hashtypes.h"
#include "client.h"
#include "data.h"

typedef int cmd_fn(struct client*, struct command*);

// hashmap: name -> function
struct map cmd_fns;

void init_command_map() {
  cmd_fns = map_init(64);
  map_put(&cmd_fns, "NICK", (void*)nick);
  map_put(&cmd_fns, "USER", (void*)user);
  map_put(&cmd_fns, "JOIN", (void*)join);
  map_put(&cmd_fns, "PING", (void*)ping);
  map_put(&cmd_fns, "PONG", (void*)pong);
}

void welcome_msg(struct client *client) {
  msg_client(client, ":glyph 001 %s :Hello, %s!\r\n", client->nick, client->nick);
  msg_client(client, ":glyph 002 %s :This server is running glyph 0.1.0\r\n", client->nick);
  msg_client(client, ":glyph 003 %s :This server was created at some point\r\n", client->nick);
  msg_client(client, ":glyph 004 %s glyph 0.1.0\r\n", client->nick);
  msg_client(client, ":glyph 005 %s CASEMAPPING=ascii CHANLIMIT=#: CHANNELLEN=32 CHANTYPES=#\r\n", client->nick);
  msg_client(client, ":glyph 422 %s :No MOTD\r\n", client->nick);
  logmsg("client %s successfully registered", client->nick);
}


// TODO: make this more uh ... better?
void handle_reg(struct client *client, struct command *cmd) {

  if (strcmp(cmd->command, "NICK") == 0 && (client->regstatus == REGSTAT_NONE || client->regstatus == REGSTAT_USER)) {
    nick(client, cmd);
    if (client->regstatus == REGSTAT_NONE) {
      client->regstatus = REGSTAT_NICK;
    } else {
      client->regstatus = REGSTAT_DONE;
      welcome_msg(client);
    }
  } else if (strcmp(cmd->command, "USER") == 0 && (client->regstatus == REGSTAT_NONE || client->regstatus == REGSTAT_NICK)) {
    user(client, cmd);
    if (client->regstatus == REGSTAT_NONE) {
      client->regstatus = REGSTAT_USER;
    } else {
      client->regstatus = REGSTAT_DONE;
      welcome_msg(client);
    }
  } else {
    msg_client(client, ":glyph 451 * :You have not registered");
  }
}

int raw_msg(struct client *client, char *buffer) {
  struct command parsed;

  parse_msg(buffer, &parsed);

  cmd_fn* fn = ((cmd_fn*)map_get(&cmd_fns, parsed.command));

  if (fn == NULL) {
    return 0;
  }

  if (client->regstatus != REGSTAT_DONE) {
    handle_reg(client, &parsed);
    tracemsg("socket %d is at regstatus %d", client->fd, client->regstatus);
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

int join(struct client *c, struct command *cmd) {
  return 0;
}

int ping(struct client *c, struct command *cmd) {
  char *token = strtok(cmd->parameters, " ");
  if (token == NULL) {
    msg_client(c, ":glyph 461\r\n");
    return 0;
  }
  msg_client(c, ":glyph PONG %s\r\n", token);
  return 0;
}

int pong(struct client *c, struct command *cmd) {
  return 0;
}

