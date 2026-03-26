  #ifndef COMMANDS_H
  #define COMMANDS_H

  #define CLIENT_COMMANDS 42

  #include "../protocol.h"

  struct command {
      char *name;   // Name des Commands für Prints etc.
      char *comm;   // Command
      char *format; // Format für Nachricht an Server
      bool exception;  // Boolean um Ellenlange If-Bedingung zu verhindern
      int size; // Maximale Größe des Inputs
      int argc; // Anzahl an Argumenten
  };


/**
 * @brief   Bekommt den Input vom Nutzer und in Form eines Commands und
 *          sendet diesen in formatierter Form an den Server
 *
 * @param sockfd    File Descriptor vom Socket
 * @param input     Eingabe des Nutzers
 * @return  Fehlercode
 */
int send_command(int *sockfd, char *input, char **host, char **port, char **nickname, char **channelname);

/**
 * @brief   Stellt Verbindung mit dem Server her
 *
 * @param input     Command Snippet aus Eingabe des Nutzers
 * @return  Nummer des Commands im Array
 */
  int what_command(char *input);

  #endif