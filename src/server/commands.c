//Befehle für den Server
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#include "commands.h"
#include "client.h"
#include "../logging.h"

#define CMD_COUNT 3

char *commands[CMD_COUNT + 1] = {
  "",
  "USER",
  "PASS",
  "NICK"
};

int raw_msg(struct client *client, char *buffer){
  struct command parsed;

  parse_msg(buffer, &parsed);
   
  if (parsed.source != NULL) {
    msg_client(client, "source: \"%s\"\n", parsed.source);
  }
  msg_client(client, "command: %d\n", parsed.command);
  msg_client(client, "parameters: \"%s\"\n", parsed.parameters);
  if (parsed.trailing != NULL) {
    msg_client(client, "trailing: \"%s\"\n", parsed.trailing);
  }

  // TODO: ...
  // msg_client(client, "Hello");
  return (0);
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

  for (int i = 1; i < CMD_COUNT; i++) {
    if (strcmp(commands[i], cmd)) {
      out->command = i;
      break;
    }
  }

  return 0;
}

