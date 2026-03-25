#ifndef DATA_H
#define DATA_H

#include "../protocol.h"
#include "../hashtypes.h"


//SERVER INFO
#define VERSION "v0.1.0"
#define AUTHORS "Vex W. & Marjan Z."
#define COMPILE_DATE "25.03.2026"

#define REGSTAT_DONE     0
#define REGSTAT_NONE     1
#define REGSTAT_USER     2
#define REGSTAT_NICK     3

struct client {
  int fd;
  char *buffer;

  char *nick;
  char *username;
  char *realname;

  struct map channels;

  int regstatus;
};

struct command {
  char *source;
  char *command;
  char *parameters;
  char *trailing;
};

struct channel {
  char name[CH_LEN];
  char topic[TOPIC_LEN];
  struct set op;
  struct map members;
};

/**
* @brief Used to INIT channel map ONCE
*/
void init_channel_map(void);

/**
* @brief 
*/
struct map* get_channel_map(void);

void create_channel(char *name, struct client *op);

void delete_channel(struct channel *channel);

void leave_channel(struct channel *channel, struct client *client);

#endif // DATA_H
