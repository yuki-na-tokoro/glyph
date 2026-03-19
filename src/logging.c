
#include "logging.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const char* levels[] = {"TRACE", "DEBUG", "LOG", "WARN", "ERROR"};

void _logmsg(uint lvl, char* msg) { 

  time_t rawtime;
  struct tm *timeinfo;
  char timestr[16];

  time(&rawtime);
  timeinfo = localtime(&rawtime);
  strftime(timestr, sizeof(timestr), "%T", timeinfo);

  if (lvl == 4) {
    fprintf(stderr, "[%s] ERROR: %s\n", timestr, msg);
    return;
  }

  const char* lvlstr;

  if (lvl > 4) {
    lvlstr = "???";
  } else {
    lvlstr = levels[lvl];
  }

  printf("[%s] %s: %s\n", timestr, lvlstr, msg);
}

void tracemsg(char* fmt, ...) {
  char* msg;
  va_list args;
  va_start(args, fmt);
  if (asprintf(&msg, fmt, args) < 0) {
    perror("#tracemsg(): asprintf error");
    exit(1);
  }
  _logmsg(TRACE, msg);
  va_end(args);
  free(msg);
}

void debugmsg(char* fmt, ...) {
  char* msg;
  va_list args;
  va_start(args, fmt);
  if (asprintf(&msg, fmt, args) < 0) {
    perror("#tracemsg(): asprintf error");
    exit(1);
  }
  _logmsg(DEBUG, msg);
  va_end(args);
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
  if (asprintf(&msg, fmt, args) < 0) {
    perror("#tracemsg(): asprintf error");
    exit(1);
  }
  _logmsg(WARN, msg);
  va_end(args);
  free(msg);
}

void errormsg(char* fmt, ...) {
  char* msg;
  va_list args;
  va_start(args, fmt);
  if (asprintf(&msg, fmt, args) < 0) {
    perror("#tracemsg(): asprintf error");
    exit(1);
  }
  _logmsg(ERROR, msg);
  va_end(args);
  free(msg);
}
