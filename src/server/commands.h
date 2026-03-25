#ifndef COMMANDS_H
#define COMMANDS_H
#include "data.h"

//TODO: MOTD command

typedef int cmd_fn(struct client*, struct command*);

/**
* @brief TODO:...
*/
void cmd_fns_put(char* cmd_name, cmd_fn fn);

/**
* @brief Used to INIT command map ONCE
*/
void init_command_map(void);

/**
*
*/
void welcome_msg(struct client *client);

/**
*
* TODO: ...
*
*/
void handle_reg(struct client *client, struct command *cmd);

/**
* @brief Parsed die raw Msg des Clients
* 
* @param client struct
* @param Buffer mit der Nachricht des Clients
*/
int raw_msg(struct client *client, char *buffer);

/**
* @brief Parsed die Msg des Clients
* 
* @param Buffer mit der Nachricht des Clients
* @param struct zum Eintragen der Daten nach dem auswerten
*/
int parse_msg(char *raw, struct command *msg);

void relay(struct client* source, char* receivers, char* raw);

/**
* @brief Setzt einen Nichnamen und vergleicht mit vorhandenen Nicknamen
*/
int nick(struct client *client, struct command *cmd);

/**
* @brief Setzt einen Nutzername bei verbindung.
*/
int user(struct client *client, struct command *cmd);

/**
* @brief Genutzt wenn ein Nutzer sich zu einem Kanal (Channel) verbinden möchte.
*
* @param user
* @param channel -> Channel name/Id
* @param key -> Passwort des Channels
*/
int join(struct client *client, struct command *cmd);

/**
*
*/
int ping(struct client *client, struct command *cmd);

/**
*
*/
int pong(struct client *client, struct command *cmd);

/**
*
*/
int version(struct client *client, struct command *cmd);

/**
*
*/
int time_cmd(struct client *client, struct command *cmd);

/**
*
*/
int info(struct client *client, struct command *cmd);

/**
* @brief Gibt eine 1 zurück falls der User c existiert. WEnn nicht eine 0.
*
* @param c -> client / user
*/
int isuser(struct client *client);

/**
* @brief Sendet eine private Nachricht an einen User
* oder wenn ziel mit einem # beginnt an einen Channel -> Liste von Usern
*
* @param c -> client/user
* @param cmd -> Command struct
*/
int privmsg(struct client *client, struct command *cmd);

/**
* @brief Bricht die Verbindung des Nutzers zu allen gegebenen Channels ab.
*
* @param user
* @param channels
*/
int part(struct client *client, struct command *cmd);

/**
*
*/
int kill(struct client *client, struct command *cmd);

/**
* @brief setzt einen USER in einem Channel auf OPERATOR
*        -> Der ausführende Nutzer muss jedoch die richtigen Permissions haben. -> muss OP sein.
* 
* @param c -> client/user
* @param a -> commands args....
*/
int oper(struct client *client, struct command *cmd);

/**
* @brief Gesendet von einem Client der die Verbindung zum Server abbrechen will
*
* @param quitmsg -> Nachricht beim verlassen.
*/
int quit(struct client *client, struct command *cmd);


/**
*
*/
int stats(struct client *client, struct command *cmd);

/**
*
*/
int notice(struct client *client, struct command *cmd);

/**
*
*/
int who(struct client *client, struct command *cmd);

/**
*
*/
int whois(struct client *client, struct command *cmd);

// int whowas(struct client *client, struct command *cmd);

/**
*
*/
int error(struct client *client, struct command *cmd);

/**
*
*/
int topic(struct client *client, struct command *cmd);

/**
*
*/
int names(struct client *client, struct command *cmd);

/**
*
*/
int list(struct client *client, struct command *cmd);

/**
*
*/
int invite(struct client *client, struct command *cmd);

/**
*
*/
int kick(struct client *client, struct command *cmd);

/**
*
*/
// int admin(struct client *client, struct command *cmd);

/**
* @brief WILL NOT BE USED FOR NOW!!
*
* @brief Setzt das Connection Passwort 
*/
// int pass(struct client *client, struct command *cmd);

#endif // COMMANDS_H
