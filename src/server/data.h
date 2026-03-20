#include "../protocol.h"

typedef struct message msg;
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

struct message {
  char *prefix;
  char *command;
  char **argv;
  int argc;
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
