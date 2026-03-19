#include "../protocol.h"

typedef struct Message {
  char *prefix;
  char *command;
  char **argv;
  int argc;
}msg;

typedef struct Channel{
  
}ch;

typedef struct User {
  
}usr;

typedef struct Membership {
 
}member;
/*
* @brief Setzt das Connection Passwort
* 
*
*/
int pass(char *password);

/**
* @brief Setzt einen Nichnamen und vergleicht mit vorhandenen Nicknamen
* 
*
*/
int nick(int *mode, char *nickname);

/**
* @brief Setzt einen Nutzername bei verbindung.
*
*/
int user(char *username, char *hostname, char *servername, char *realname);

/**
* @brief setzt einen USER mit richtigem PASS auf OPERATOR
*        -> Der Nutzer muss jedoch die richtigen Permissions haben. 
* 
* @param username -> Nutzername
* @param pass -> Passwort des nutzers
*/
int oper(char *username, char *pass);

/**
* @brief Gesendet von einem Client der die Verbindung zum Server abbrechen will
*
* @param quitmsg -> Nachricht beim verlassen.
*/
int quit(char *quitmsg);


/*
*######################################
* Channel Operations
*######################################
*/

/**
* @brief Genutzt wenn ein Nutzer sich zu einem Kanal (Channel) verbinden möchte.
*
* @param user
* @param channel -> Channel name/Id
* @param key -> Passwort des Channels
*/
int join(char *user, char *channel, char * key);

/*
* @brief Bricht die Verbindung des Nutzers zu allen gegebenen Channels ab.
*
* @param user
* @param channels
*/
int part(char * user, char *channels);

/*
*######################################
* Server Queries and Commands
*######################################
*/

/*
*
*
*
*/
int version();

/*
*
*
*
*/
int stats();

/*
*
*
*
*/

int time();

/*
*
*
*
*/
int admin();

/*
*
*
*
*/
int info();

/*
*
*
*
*/
int privmsg();

/*
*
*
*
*/
int notice();

/*
*
*
*
*/
int who();

/*
*
*
*
*/
int whois();

/*
*
*
*
*/
int whowas();

/*
*
*
*
*/
int kill();

/*
*
*
*
*/
int ping();

/*
*
*
*
*/
int pong();

/*
*
*
*
*/
int error();

/*
*
*
*
*/
int topic();

/*
*
*
*
*/
int names();

/*
*
*
*
*/
int list();

/*
*
*
*
*/
int invite();

/*
*
*
*
*/
int kick();
