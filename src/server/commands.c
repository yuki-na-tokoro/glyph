//Befehle für den Server
#include <stdarg.h>
#include <stdarg.h>
#include <stdbool.h>
// #include <stdio.h>
#include <stdio.h>
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
  cmd_fns_put("VERSION", version);
  cmd_fns_put("PRIVMSG", privmsg);
  cmd_fns_put("PART", part);
  logmsg("#init_command_map(): done");
}

void welcome_msg(struct client *client) {
  msg_client(client, ":glyph 001 %s :Hello, %s!\r\n", client->nick, client->nick);
  msg_client(client, ":glyph 002 %s :This server is running glyph %s\r\n", client->nick, VERSION);
  msg_client(client, ":glyph 003 %s :This server was created at some point\r\n", client->nick);
  msg_client(client, ":glyph 004 %s glyph %s\r\n", client->nick, VERSION);
  msg_client(client, ":glyph 005 %s CASEMAPPING=ascii CHANLIMIT=#: CHANNELLEN=32 CHANTYPES=#\r\n", client->nick);
  msg_client(client, ":glyph 422 %s :No MOTD\r\n", client->nick);
  logmsg("#welcome_msg(): client %s successfully registered", client->username);
}


// TODO: make this more uh ... better?
void handle_reg(struct client *client, struct command *cmd) {

  if (strcmp(cmd->command, "NICK") == 0 && (client->regstatus == REGSTAT_NONE || client->regstatus == REGSTAT_USER)) {

    char *nick = strtok(cmd->parameters, " ");

    if (set_contains(get_nicks(), nick)) {
      // TODO: reply
      return; 
    }

    client->nick = nick;

    set_put(get_nicks(), nick);

    if (client->regstatus == REGSTAT_NONE) {
      client->regstatus = REGSTAT_NICK;
    } else {
      client->regstatus = REGSTAT_DONE;
      create_channel(client->nick, client);
      welcome_msg(client);
    }
  } else if (strcmp(cmd->command, "USER") == 0 && (client->regstatus == REGSTAT_NONE || client->regstatus == REGSTAT_NICK)) {

    char *username = strtok(cmd->parameters, " ");

    if (set_contains(get_users(), username)) {
      // TODO: reply
      return; 
    }

    client->username = username;
    client->realname = cmd->trailing;

    set_put(get_users(), username);

    if (client->regstatus == REGSTAT_NONE) {
      client->regstatus = REGSTAT_USER;
    } else {
      client->regstatus = REGSTAT_DONE;
      create_channel(client->nick, client);
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

  if ((out->raw = calloc(strlen(raw) + 1, sizeof(char))) == NULL) {
    // TODO: error handling
  }

  strcpy(out->raw, raw);

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

struct map channels_to_receivers(struct map* channel_map) {

  struct map rec_map = map_init(64);

  for (size_t i = 0; i < channel_map->cap; i++)  {

    struct channel *channel = channel_map->buckets[i].value;

    if (channel == NULL) continue;

    for (size_t j = 0; j < channel->members.cap; j++) {

      struct client *member = channel->members.buckets[j].value;

      if (member == NULL) continue;
      
      map_put(&rec_map, member->username, member);
    }
  }

  return rec_map;
}

struct map parse_receivers(struct client* source, char* receivers) {
  
  char *receiver = strtok(receivers, ",");

  struct map rec_map = map_init(64);

  while(receiver != NULL) { //Ietriere über alle Channel die als Parameter gegeben sind

    struct channel *channel = map_get(&source->channels, receiver); 
    
    if (channel == NULL) {
      receiver = strtok(NULL, ",");
      continue;
    }

    for (size_t i = 0; i < channel->members.cap; i++) {
      struct client *member = channel->members.buckets[i].value;

      if (member == NULL) continue;

      map_put(&rec_map, member->username, member);
    }

    tracemsg("next: %s", receiver);
    receiver = strtok(NULL, ",");
  }

  return rec_map;
}

void relay(struct client* source, bool include, struct map* rec_map, char* raw) {

  for (size_t i = 0; i < rec_map->cap; i++) {

    struct client *member = rec_map->buckets[i].value;

    if (member == NULL || (!include && strcmp(source->username, member->username) == 0)) continue;

    msg_client(member, ":%s %s\r\n", source->nick, raw);
  }
}

void relay_multi(struct client *source, char* fmt, char *receivers, char* trailing) {

  char *receiver = strtok(receivers, ",");

  while(receiver != NULL) { //Iteriere über alle Channel die als Parameter gegeben sind

    struct channel *channel = map_get(&source->channels, receiver); 
    
    if (channel == NULL) {
      receiver = strtok(NULL, ",");
      continue;
    }

    for (size_t i = 0; i < channel->members.cap; i++) {

      struct client *member = channel->members.buckets[i].value;

      if (member == NULL) continue;

      char *msg;

      if (asprintf(&msg, fmt, channel->name, trailing) < 0) {
        // TODO: error handling
      }

      msg_client(member, ":%s %s\r\n", source->nick, msg);

      free(msg);

    }

    receiver = strtok(NULL, ",");
  }
}


int nick(struct client *c, struct command *cmd) {
  char *nick = strtok(cmd->parameters, " ");

  if (set_contains(get_nicks(), nick)) {
    // TODO: reply
    return 0;
  }

  set_remove(get_nicks(), c->nick);
  set_put(get_nicks(), nick);

  leave_channel(map_get(get_channel_map(), c->nick), c);

  struct map rec_map = channels_to_receivers(&c->channels);
  relay(c, true, &rec_map, cmd->raw);
  free(rec_map.buckets);

  c->nick = nick;

  create_channel(nick, c);
  
  return 0;
}

int user(struct client *c, struct command *cmd) {
  // TODO: decline
  return 0;
}

int join(struct client *client, struct command *cmd) {

  char *channels = strtok(cmd->parameters, " ");

  char *channel_name = strtok(channels, ",");

  struct set channel_set = set_init(16);

  while (channel_name != NULL) {

    if (channel_name[0] != '#') continue;

    struct channel *channel = map_get(get_channel_map(), channel_name);

    if (channel == NULL) {
      create_channel(channel_name, client);
    } else {
      map_put(&channel->members, client->username, client);
      map_put(&client->channels, channel_name, channel);
    };

    set_put(&channel_set, channel_name);

    channel_name = strtok(NULL, ",");
    tracemsg("next channel: %s", channel_name);
  }

  for (size_t i = 0; i < channel_set.cap; i++) {
    if (channel_set.buckets[i] == NULL) continue;

    relay_multi(client, "JOIN %s", channel_set.buckets[i], "");
  }

  free(channel_set.buckets);

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
  msg_client(c, "   ______ __              __\n"
                "  / ____// /_  __ ____   / /_\n"
                " / / __ / / / / // __ \\ / __ \\\n"
                "/ /_/ // / /_/ // /_/ // / / /\n"
                "\\____//_/\\_   // .___//_/ /_/\n"
                "        /____//_/\n"
                "+------------------------------+\n"
                "|   GLYPH IRC SERVER %-8s  |\n"
                "+------------------------------+\n"
                " \n"
                "made by: %s\n"
                "compile date: %s\r\n",VERSION,AUTHORS,COMPILE_DATE);
  return 0;
}

int isuser(struct client *cmd){
  // TODO: check if user exists
  return 1;
}

int privmsg(struct client *client, struct command *cmd){

  if (cmd->trailing == NULL){
    tracemsg("no message provided to PRIVMSG");
    // TODO: corect error msg
    msg_client(client,":Server %s : missing arguments!\r\n",cmd->command);
    return 0;
  }

  char *receivers, *receiver;
  receivers = strtok(cmd->parameters, " ");

  tracemsg("#privmsg(): %s -> %s", client->username, receivers);

  struct map rec_map = parse_receivers(client, receivers);

  relay(client, false, &rec_map, cmd->raw);

  free(rec_map.buckets);

  return 0;
}

int part(struct client *client, struct command *cmd){
  /**
  * CHECK INPUT/ERROR HANDLING
  */
  if (cmd->parameters== NULL){
    return 0;
  }

  /**
  * FUNCTION: Iteriere ueber @paramater(channel die verlassen werden sollen) und verlasse sie,
  *           wenn sie ein gueltiger Channel sind. 
  */
  char *channels = strtok(cmd->parameters, " ");

  char *channel_name = strtok(channels, ",");

  struct set channel_set = set_init(16);

  while (channel_name != NULL) {

    if (channel_name[0] != '#') continue;

    struct channel *channel = map_get(get_channel_map(), channel_name);

    if (channel == NULL) continue;

    tracemsg("%s parting from %s", client->username, channel_name);

    set_put(&channel_set, channel_name);

    leave_channel(channel, client);

    channel_name = strtok(NULL, ",");
  }

  for (size_t i = 0; i < channel_set.cap; i++) {
    if (channel_set.buckets[i] == NULL) continue;

    if (cmd->trailing != NULL) {
      relay_multi(client, "PART %s :%s", channel_name, cmd->trailing);
    } else {
      relay_multi(client, "PART %s", channel_name, "");
    }
  }

  free(channel_set.buckets);

  return 0;
}

int kill(struct client *c, struct command *a){
  return 0;
}
