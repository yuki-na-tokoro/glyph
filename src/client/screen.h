  #ifndef SCREEN_H
  #define SCREEN_H

  #include "../protocol.h"

  extern WINDOW *chatfeld;
  extern WINDOW *eingabefeld;
  void setupscreen();

  void chat_print(const char *text, ...);

  void get_eingabe(char *buf);

  #endif
  