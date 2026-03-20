//Befehle für den Server
#include <stdbool.h>
#include <unistd.h>

#include "commands.h"
#include "client.h"

int raw_msg(struct client *client, char *buffer){
  struct Message parsed;

  parsed.prefix = NULL;
  parsed.command = NULL;
  parsed.argc = 0;

  parse_msg(buffer, parsed);
 
  // TODO: ...
}

void parse_msg(char *raw, struct Message msg) {
  
}

int pass( struct User *usr, char *password){
  
}

int nick(int *mode, char *nickname);

int user(char *username, char *hostname, char *servername, char *realname);

int oper(char *username, char *pass);

int quit(char *quitmsg);

int join(char *user, char *channel, char * key);

int part(char * user, char *channels);
