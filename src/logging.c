
#include "logging.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define RED(s)     "\x1b[31m"s"\x1b[0m"
#define BRED(s)    "\x1b[91m"s"\x1b[0m"
#define GREEN(s)   "\x1b[32m"s"\x1b[0m"
#define BGREEN(s)  "\x1b[92m"s"\x1b[0m"
#define YELLOW(s)  "\x1b[33m"s"\x1b[0m"
#define BYELLOW(s) "\x1b[93m"s"\x1b[0m"
#define BLUE(s)    "\x1b[34m"s"\x1b[0m"
#define BBLUE(s)   "\x1b[94m"s"\x1b[0m"
#define CYAN(s)    "\x1b[35m"s"\x1b[0m"
#define BWHITE(s)  "\x1b[97m"s"\x1b[0m"

#define DIMMED(s)  "\x1b[2m"s"\x1b[0m"

const char* levels[] = {BWHITE("TRACE"), BBLUE("DEBUG"), BGREEN("LOG"), BYELLOW("WARN"), BRED("ERROR")};

void _logmsg(uint lvl, char* msg) { 

  if (lvl < MIN_LEVEL) return;

  time_t rawtime;
  struct tm *timeinfo;
  char timestr[16];

  time(&rawtime);
  timeinfo = localtime(&rawtime);
  strftime(timestr, sizeof(timestr), "%T", timeinfo);

  if (lvl == 4) {
    fprintf(stderr, "[" DIMMED("%s") "] (ERROR): %s\n", timestr, msg);
    return;
  }

  const char* lvlstr;

  if (lvl > 4) {
    lvlstr = "???";
  } else {
    lvlstr = levels[lvl];
  }

  printf("[" DIMMED("%s") "] (%s): %s\n", timestr, lvlstr, msg);
}

void tracemsg(char* fmt, ...) {
  char* msg;
  va_list args;
  va_start(args, fmt);
  if (vasprintf(&msg, fmt, args) < 0) {
    perror("#tracemsg(): asprintf error");
    va_end(args);
    return;
  }

  va_end(args);
  _logmsg(TRACE, msg);
  free(msg);
}

void debugmsg(char* fmt, ...) {
  char* msg;
  va_list args;
  va_start(args, fmt);
  if (vasprintf(&msg, fmt, args) < 0) {
    perror("#tracemsg(): asprintf error");
    va_end(args);
    return;
  }

  va_end(args);
  _logmsg(DEBUG, msg);
  free(msg);
}

void logmsg(char* fmt, ...) {
  char* msg;
  va_list args;
  va_start(args, fmt);
  if (vasprintf(&msg, fmt, args) < 0) {
    perror("#tracemsg(): asprintf error");
    va_end(args);
    return;
  }

  va_end(args);
  _logmsg(LOG, msg);
  free(msg);
}

void warnmsg(char* fmt, ...) {
  char* msg;
  va_list args;
  va_start(args, fmt);
  if (vasprintf(&msg, fmt, args) < 0) {
    perror("#tracemsg(): asprintf error");
    va_end(args);
    return;
  }

  va_end(args);
  _logmsg(WARN, msg);
  free(msg);
}

void errormsg(char* fmt, ...) {
  char* msg;
  va_list args;
  va_start(args, fmt);
  if (vasprintf(&msg, fmt, args) < 0) {
    perror("#tracemsg(): asprintf error");
    va_end(args);
    return;
  }

  va_end(args);
  _logmsg(ERROR, msg);
  free(msg);
}
