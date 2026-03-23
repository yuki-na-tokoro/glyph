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
   
  // TODO: ...
  msg_client(client, "Hello");
  return (0);
}

int parse_msg(char *raw, struct command *out) {

  char *source = NULL;
  char *cmd;
  char *params = strtok(raw, " :");
  char *trailing = strtok(NULL, "");

  if (params[0] == '@') {
    strtok(params, " ");
    params = strtok(NULL, "");
  }
  if (params[0] == ':') {
    source = strtok(params, " ");
  }

  cmd = strtok(NULL, " ");
  params = strtok(NULL, "");
  
  out->source = source;
  out->params = params;
  out->trailing = trailing;

  for (int i = 1; i < CMD_COUNT; i++) {
    if (strcmp(commands[i], cmd)) {
      out->command = i;
      break;
    }
  }

  return 0;
}

