#ifndef COMMANDS_H
#define COMMANDS_H
#include "data.h"

typedef int cmd_fn(struct client*, struct command*);

struct cmd_fn_container {
  cmd_fn *fn;
};

/**
* @brief adds the commands to the commadns map
*
* @param cmd_name -> name of the command
* @param fn -> command function
*/
void cmd_fns_put(char* cmd_name, cmd_fn fn);

/**
* @brief Used to INIT command map ONCE
*/
void init_command_map(void);

/**
* @brief sends the Welcome msg to the Client upon successful registration.
*
* @param client -> client to send the message to.
*/
void welcome_msg(struct client *client);

/**
* @brief Checks if registration is finished else prompts for client to finish registration.
*
* @param client -> lient to check for registration
* @param cmd -> command the client sent
*/
void handle_reg(struct client *client, struct command *cmd);

/**
* @brief parses the raw message sent by the client.
* 
* @param client -> client struct
* @param buffer -> buffer with the raw message of the client.
*/
int raw_msg(struct client *client, char *buffer);

/**
* @brief parses the parsed message into a command
* 
* @param raw -> buffer with the client 
* @param msg -> struct zum Eintragen der Daten nach dem auswerten
*/
int parse_msg(char *raw, struct command *msg);

/**
* @brief Takes channel as arg and return alls user form these channels
*
* @param source -> client where the channels are gotten from -> has to be in the channels form the args
* @param recievers -> recieving channels to be scanned
*/
struct map parse_receivers(struct client* source, char* receivers);

/**
* @brief Relays a message to given clients
*
* @param source -> client to be used as sender
* @param include -> should the sender als recieve the message 
* @param rec_map -> map of recievers of the message
* @param raw -> message to be sent to the users
*/
void relay(struct client* source, bool include, struct map* rec_map, char* raw);

/**
* 
*/
void relay_multi(struct client *source, bool include, char* fmt, char *receivers, char* trailing);

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
*/
int ping(struct client *client, struct command *cmd);

/**
*/
int pong(struct client *client, struct command *cmd);

/**
*/
int version(struct client *client, struct command *cmd);

/**
*/
int time_cmd(struct client *client, struct command *cmd);

/**
*/
int info(struct client *client, struct command *cmd);

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
************************************************************************
* !!! @NOTICE: UNIMPLEMENTED UND UNUSED COMMANDS AFTER THIS COMMENT !!!
************************************************************************
*
*/

/**
* @brief Gesendet von einem Client der die Verbindung zum Server abbrechen will
*
* @param quitmsg -> Nachricht beim verlassen.
*/
int quit(struct client *client, struct command *cmd);

int stats(struct client *client, struct command *cmd);

int notice(struct client *client, struct command *cmd);

int list(struct client *client, struct command *cmd);

int kick(struct client *client, struct command *cmd);

/**
* @brief setzt einen USER in einem Channel auf OPERATOR
*        -> Der ausführende Nutzer muss jedoch die richtigen Permissions haben. -> muss OP sein.
* 
* @param c -> client/user
* @param a -> commands args....
*/
int oper(struct client *client, struct command *cmd);

#endif // COMMANDS_H
