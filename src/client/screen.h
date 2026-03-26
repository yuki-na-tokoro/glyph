  #ifndef SCREEN_H
  #define SCREEN_H

  #include "../protocol.h"

  extern WINDOW *chatfeld;  // Ausgabefeld wo chat_print ausgibt
  extern WINDOW *eingabefeld; // Feld wo der Nutzer Eingaben tätigt
  extern WINDOW *infos; // Leiste mit Infos über dem Eingabefeld

  /**
 * @brief   Initialisiert den Screen
 */
  void setupscreen();


/**
 * @brief   Printed Ausgaben mit Timestamp
 *
 * @param text  Input
 */
  void chat_print(const char *text, ...);


/**
 * @brief   Printed Ausgaben ohne Timestamp
 *
 * @param text  Input
 */
  void timeless_chat_print(const char *text, ...);

  #endif
  