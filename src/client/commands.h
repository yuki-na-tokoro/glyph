  #ifndef COMMANDS_H
  #define COMMANDS_H

  #define CLIENT_COMMANDS 17

  #include "../protocol.h"

  struct command {
      char *name;   // Name des Commands für Prints etc.
      char *comm;   // Command
      char *format; // Format für Nachricht an Server
      int size; // Maximale Größe des Inputs
      int argc; // Anzahl an Argumenten
  };

/**
 * @brief   Bekommt den Input vom Nutzer und in Form eines Commands und
 *          sendet diesen in formatierter Form an den Server
 *
 * @param sockfd    File Descriptor vom Socket
 * @param input     Eingabe des Nutzers
 */
void send_command(int sockfd, char *input);

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