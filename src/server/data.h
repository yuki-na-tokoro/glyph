#ifndef DATA_H
#define DATA_H

#include "../protocol.h"
#include "../hashtypes.h"

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
  struct client *op;
  struct set members;
};

#endif // DATA_H
