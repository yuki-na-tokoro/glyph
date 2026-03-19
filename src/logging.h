#ifndef LOGGING_H
#define LOGGING_H

#include <sys/types.h>

#define TRACE 0
#define DEBUG 1
#define LOG   2
#define WARN  3
#define ERROR 4

void _logmsg(uint level, char* msg);

void tracemsg(char* msg, ...);
void debugmsg(char* msg, ...);
void logmsg(char* msg, ...);
void warnmsg(char* msg, ...);
void errormsg(char* msg, ...);

#endif
