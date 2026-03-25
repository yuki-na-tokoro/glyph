//Befehle für den Server
#include <stdbool.h>
// #include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "commands.h"
#include "../logging.h"
#include "../hashtypes.h"
#include "client.h"
#include "data.h"
// #include "numerics.h"

struct map cmd_fns;

void cmd_fns_put(char* cmd_name, cmd_fn fn) {
  map_put(&cmd_fns, cmd_name, (void*)fn);
}

void init_command_map() {
  cmd_fns = map_init(64);
  cmd_fns_put("NICK", nick);
  cmd_fns_put("USER", user);
  cmd_fns_put("JOIN", join);
  cmd_fns_put("PING", ping);
  cmd_fns_put("PONG", pong);
  cmd_fns_put("INFO", info);
  cmd_fns_put("TIME", time_cmd);
  cmd_fns_put("PRIVMSG", privmsg);
  logmsg("#init_command_map(): done");
}

void welcome_msg(struct client *client) {
  msg_client(client, ":glyph 001 %s :Hello, %s!\r\n", client->nick, client->nick);
  msg_client(client, ":glyph 002 %s :This server is running glyph %s\r\n", client->nick, VERSION);
  msg_client(client, ":glyph 003 %s :This server was created at some point\r\n", client->nick);
  msg_client(client, ":glyph 004 %s glyph %s\r\n", client->nick, VERSION);
  msg_client(client, ":glyph 005 %s CASEMAPPING=ascii CHANLIMIT=#: CHANNELLEN=32 CHANTYPES=#\r\n", client->nick);
  msg_client(client, ":glyph 422 %s :No MOTD\r\n", client->nick);
  logmsg("#welcome_msg(): client %s successfully registered", client->nick);
}


// TODO: make this more uh ... better?
void handle_reg(struct client *client, struct command *cmd) {

  if (strcmp(cmd->command, "NICK") == 0 && (client->regstatus == REGSTAT_NONE || client->regstatus == REGSTAT_USER)) {
    nick(client, cmd);
    if (client->regstatus == REGSTAT_NONE) {
      client->regstatus = REGSTAT_NICK;
    } else {
      client->regstatus = REGSTAT_DONE;
      create_channel(client->username, client);
      welcome_msg(client);
    }
  } else if (strcmp(cmd->command, "USER") == 0 && (client->regstatus == REGSTAT_NONE || client->regstatus == REGSTAT_NICK)) {
    user(client, cmd);
    if (client->regstatus == REGSTAT_NONE) {
      client->regstatus = REGSTAT_USER;
    } else {
      client->regstatus = REGSTAT_DONE;
      create_channel(client->username, client);
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

  free(parsed.command);
  
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
    if ((body = malloc(strlen(raw) * sizeof(char) + 1)) == NULL) {
      // TODO: error handling
    }
    strcpy(body, raw);
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

  char *channels = strtok(cmd->parameters, " ");
  char *channel_name = strtok(channels, ",");

  while (channel_name != NULL) {
    tracemsg("channel: %s", channel_name);
    if (map_get(get_channel_map(), channel_name) == NULL) {
      tracemsg("creating channel %s", channel_name);

      create_channel(channel_name, c);

    } else {

      struct channel *channel = map_get(&c->channels, channel_name);

      map_put(&channel->members, c->nick, c);
      map_put(&c->channels, channel->name, channel);
    }

    channel_name = strtok(NULL, ",");
  }

  return 0;
}

int ping(struct client *c, struct command *cmd) {
  char *token = strtok(cmd->parameters, " ");
  if (token == NULL) {
    msg_client(c, ":glyph 461\r\n");
    return 1;
  }
  msg_client(c, ":glyph PONG %s\r\n", token);
  return 0;
}

int pong(struct client *c, struct command *cmd) {
  return 0;
}


int version(struct client *c, struct command *a){
  msg_client(c, "Current glyph version is :%s", VERSION);  
  return 0;
}

int time_cmd(struct client *c, struct command *cmd){
  if (cmd->parameters != NULL || cmd->trailing != NULL){
    tracemsg("Invalid params provided to TIME");
    // TODO: corect error msg
    msg_client(c,"Too Many Arguments!\r\n");
    return 1;
  }
  time_t rawtime;
  struct tm *timeinfo;
  char timestr[80];
  
  time(&rawtime);
  timeinfo = localtime(&rawtime);

  strftime(timestr,sizeof(timestr),"%Y.%m.%d - %H:%M:%S", timeinfo);
  
  msg_client(c,"Server localtime is: %s \r\n",timestr);
  return 0;  
}

int info(struct client *c, struct command *cmd){
  if (cmd->parameters != NULL || cmd->trailing != NULL){
    tracemsg("Invalid params provided to INFO");
    // TODO: corect error msg
    msg_client(c,":Server %s : Too many args!\r\n",cmd->command);
    return 1;
  }
  msg_client(c, "   ______ __              __  \n"
                "  / ____// /_  __ ____   / /_  \n"
                " / / __ / / / / // __ \\ / __ \\ \n"
                "/ /_/ // / /_/ // /_/ // / / / \n"
                "\\____//_/\\__, // .___//_/ /_/  \n"
                "        /____//_/              \n"
                "+------------------------------+\n"
                "|   GLYPH IRC SERVER %-8s  |\n"
                "+------------------------------+\n\r\n",VERSION);
  // msg_client(c,"glyph IRC Server Version:%s\r\n",VERSION);
  msg_client(c,"made by: %s\r\n",AUTHORS);
  msg_client(c,"compile date: %s\r\n",COMPILE_DATE);
  return 0;
}

int isuser(struct client *cmd){
  // TODO: check if user exists
  return 1;
}

int privmsg(struct client *c, struct command *cmd){
  if (cmd->trailing == NULL){
    tracemsg("no message provided to PRIVMSG");
    // TODO: corect error msg
    msg_client(c,":Server %s : missing arguments!\r\n",cmd->command);
    return 0;
  }
  char *param = cmd->parameters;
  if(strcmp(strchr(param,1),"#")== 0){
    // TODO: Send message to allf rom channel
  }
  
  return 0;
}

int kill(struct client *c, struct command *a){
  return 0;
}
  // close(clients[i]->fd);
  // free(clients[i]);

int quit(struct client *c, struct command *a){
  return 0;
}

int part(struct client *c, struct command *a){
  return 0;
}

int topic(struct client *c, struct command *a){
  return 0;
}

int names(struct client *c, struct command *a){
  return 0;
}

int list(struct client *c, struct command *a){
  return 0;
}

/**
* LOW PRIORITY COMMANDS
*/

// int error(struct client *c, struct command *a);

// int kick(struct client *c, struct command *a);

// int oper(struct client *c, struct command *a){
//   return 0; 
// }

// int notice(struct client *c, struct command *a);

// int who(struct client *c, struct command *a);

// int whois(struct client *c, struct command *a);

