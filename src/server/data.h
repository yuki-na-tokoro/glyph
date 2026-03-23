#ifndef DATA_H
#define DATA_H

#include "../protocol.h"

struct client {
  int fd;
  char *buffer;

  char nick[NICK_LEN];
  char username[USER_LEN];
  char *realname;

  struct map *channels;
};

enum cmd_id {
  USER = 1,
  PASS = 2,
  NICK = 3,
};

struct command {
  char *source;
  int command;
  char *parameters;
  char *trailing;
};

struct channel {
  char name[CH_LEN];
  char topic[TOPIC_LEN];
  struct client *op;
  struct map *members;
};

#endif // DATA_H
