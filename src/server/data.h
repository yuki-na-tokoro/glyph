#ifndef DATA_H
#define DATA_H

#include "../protocol.h"

typedef struct command cmd;
typedef struct channel ch;
typedef struct membership member;

struct client {
  int fd;
  int id[ID_LEN+1];
  char* buffer;
  char nick[NICK_LEN];
  char username[USER_LEN];
  struct sockaddr *addr;

  struct membership *channels;
  struct client *next; 
};

enum C_ID {
  USER = 1,
  PASS = 2,
  NICK = 3,
};

struct command {
  char *source;
  int command;
  char *params;
  char *trailing;
};

struct channel{
  char name[CH_LEN];
  char topic[TOPIC_LEN];
  struct membership *members;
  struct channel *next;
};

struct membership {
  struct User *user;
  struct channel *channel;

  bool is_op;
  
  struct membership *next_user;
  struct membership *next_channel;
};

#endif // DATA_H
