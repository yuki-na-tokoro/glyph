//Befehle für den Server
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "commands.h"
#include "client.h"
#include "../logging.h"

#define CMD 3

struct dict {
  unsigned int id;
  char *command;
};

struct dict commands[3] = {
  {1,"USER"},
  {2,"PASS"},
  {3,"NICK"},
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
  char *ms[2];
  char *a = strtok(raw,":");


  /**
  * Filter out the Optional stuff
  */
  for(int i = 0;i<2;i++){
    if (a == NULL) break;
    
    if (a[0]== '@' || a[0]== 64){
       ms[0] = a;
    }
    else {
      ms[1]=a;
      break;
    }
    a = strtok(NULL,":");
  }
  if (ms[1]!=NULL){
    tracemsg("Recieved Message  Body : %s", ms[1]); 
  }else{
    tracemsg("Recieved no Message Body !");
  }
  /**
  * Parse Message Body/Command
  */
  a = strtok(ms[1]," ");
  for(int i = 0;i<CMD;i++){
    if (a == commands[i].command){
      out->command_id = commands[i].id;
    }
  }
  a = strtok(NULL," ");
  for(int i = 0;i<CMD;i++){
    if (a == commands[i].command){
      out->command_id = commands[i].id;
    }
  }
}

