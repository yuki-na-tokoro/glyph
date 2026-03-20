#ifndef LOGGING_H
#define LOGGING_H

#include <sys/types.h>

#define TRACE 0
#define DEBUG 1
#define LOG   2
#define WARN  3
#define ERROR 4

/**
 * @brief schreibt formatierte log nachrichten
 *
 * @param level  log level
 * @param msg    log nachricht
 */
void _logmsg(uint level, char* msg);

/**
 * @brief schreibt formatierte log nachrichten auf trace level
 *
 * @param msg  log nachricht
 */
void tracemsg(char* msg, ...);

/**
 * @brief schreibt formatierte log nachrichten auf debug level
 *
 * @param msg  log nachricht
 */
void debugmsg(char* msg, ...);

/**
 * @brief schreibt formatierte log nachrichten auf log level
 *
 * @param msg  log nachricht
 */
void logmsg(char* msg, ...);

/**
 * @brief schreibt formatierte log nachrichten auf warn level
 *
 * @param msg  log nachricht
 */
void warnmsg(char* msg, ...);

/**
 * @brief schreibt formatierte log nachrichten auf error level
 *
 * @param msg  log nachricht
 */
void errormsg(char* msg, ...);

#endif
