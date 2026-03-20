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

struct command {
  int command_id;
  int argc;
  char *argv[15];
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
