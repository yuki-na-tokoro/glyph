  #ifndef COMMANDS_H
  #define COMMANDS_H

  #define CLIENT_COMMANDS 18

  #include "../protocol.h"

  struct command {
      char *name;   // Name des Commands für Prints etc.
      char *comm;   // Command
      char *format; // Format für Nachricht an Server
      bool channel;  // Channel in Nachricht
      int size; // Maximale Größe des Inputs
      int argc; // Anzahl an Argumenten
  };

/**
 * @brief   Fordert den Nutzer zur Eingabe auf, überprüft diese und übergibt
 *
 * @return input     Eingabe des Nutzers
 */  
char *get_input(void);

/**
 * @brief   Bekommt den Input vom Nutzer und in Form eines Commands und
 *          sendet diesen in formatierter Form an den Server
 *
 * @param sockfd    File Descriptor vom Socket
 * @param input     Eingabe des Nutzers
 */
int send_command(int *sockfd, char *input, char **host, char **port, char **nickname, char **channelname);

/**
 * @brief   Stellt Verbindung mit dem Server her
 *
 * @param input     Command Snippet aus Eingabe des Nutzers
 * @return  Nummer des Commands im Array
 */
  int what_command(char *input);

  extern struct command commands[];
  extern int commands_len;

  #endif