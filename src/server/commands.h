#ifndef COMMANDS_H
#define COMMANDS_H
#include "data.h"

//TODO: MOTD command

/**
* @brief Used to INIT channel map ONCE
*/
void init_channel_map(void);

/**
* @brief Used to INIT command map ONCE
*/
void init_command_map(void);

/**
* @brief Parsed die raw Msg des Clients
* 
* @param client struct
* @param Buffer mit der Nachricht des Clients
*/
int raw_msg(struct client *client, char *buffer);

/**
* @brief Parsed die raw Msg des Clients
* 
* @param Buffer mit der Nachricht des Clients
* @param struct zum Eintragen der Daten nach dem auswerten
*/
int parse_msg(char *raw, struct command *msg);

/**
*
*/
int welcome(struct client *c);
/**
* @brief WILL NOT BE USED FOR NOW!!
*
* @brief Setzt das Connection Passwort 
*/
int pass(struct client *c, struct command *a);

/**
* @brief Setzt einen Nichnamen und vergleicht mit vorhandenen Nicknamen
*/
int nick(struct client *c, struct command *a);

/**
* @brief Setzt einen Nutzername bei verbindung.
*/
int user(struct client *c, struct command *a);

/**
* @brief setzt einen USER in einem Channel auf OPERATOR
*        -> Der ausführende Nutzer muss jedoch die richtigen Permissions haben. -> muss OP sein.
* 
* @param c -> client/user
* @param a -> commands args....
*/
int oper(struct client *c, struct command *a);

/**
* @brief Gesendet von einem Client der die Verbindung zum Server abbrechen will
*
* @param quitmsg -> Nachricht beim verlassen.
*/
int quit(struct client *c, struct command *a);

/**
* @brief Genutzt wenn ein Nutzer sich zu einem Kanal (Channel) verbinden möchte.
*
* @param user
* @param channel -> Channel name/Id
* @param key -> Passwort des Channels
*/
int join(struct client *c, struct command *a);

/**
* @brief Bricht die Verbindung des Nutzers zu allen gegebenen Channels ab.
*
* @param user
* @param channels
*/
int part(struct client *c, struct command *a);

/**
*/
int version(struct client *c, struct command *a);

/**
*/
int stats(struct client *c, struct command *a);

/**
*
*/

int time_cmd(struct client *c, struct command *a);

/**
*
*/
// int admin(struct client *c, struct command *a);

/**
*
*/
int info(struct client *c, struct command *a);

/**
*
*/
int privmsg(struct client *c, struct command *a);

/**
*
*/
int notice(struct client *c, struct command *a);

/**
*
*/
int who(struct client *c, struct command *a);

/**
*
*/
int whois(struct client *c, struct command *a);

// int whowas(struct client *c, struct command *a);

/**
*
*/
int kill(struct client *c, struct command *a);

/**
*
*/
int ping(struct client *c, struct command *a);

/**
*
*/
int pong(struct client *c, struct command *a);

/**
*
*/
int error(struct client *c, struct command *a);

/**
*
*/
int topic(struct client *c, struct command *a);

/**
*
*/
int names(struct client *c, struct command *a);

/**
*
*/
int list(struct client *c, struct command *a);

/**
*
*/
int invite(struct client *c, struct command *a);

/**
*
*/
int kick(struct client *c, struct command *a);

#endif // COMMANDS_H
