//Befehle für den Server
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "commands.h"
#include "client.h"

#define CMD 2

struct dict {
  unsigned int id;
  char *command;
};

struct dict commands[2] = {
  {1,"USER"},
  {2,"PASS"},
};
enum commands{
  USER = 1,
  PASS = 2,
  NICK = 3,
};

int raw_msg(struct client *client, char *buffer){
  struct command parsed;

  parsed.command_id = 0;
  parsed.argc = 0;
  for( int i = 0; i < 15; i++){
    parsed.argv[i] = NULL; 
  }
  
  parse_msg(buffer, &parsed);
   
  // TODO: ...
  msg_client(client, "Hello");
  return (0);
}

void parse_msg(char *raw, struct command *out) {
  char *a = strtok(raw," ");
  
  for(int i = 0;i < CMD;i++){
    if (strcmp(commands[i].command, a) == 0)out->command_id = commands[i].id;
  }
  
  for(;;){
    char *temp;
    if(out->argv[0] == NULL){
      temp = strtok(raw," ");
    }
    else temp = strtok(NULL," ");
    
    if (temp == NULL) break;

    else {
      out->argv[out->argc] = temp;
      out->argc++;
    };
  };
}
