#ifndef NETWORKING_H
#define NETWORKING_H

#include <sys/poll.h>

#include "client.h"

#define BACKLOG 10
#define MAX_CONN 128
#define TIMEOUT 3 * 60 * 1000

/**
 * @brief initializiert den server socket mit #bind() und #listen()
 *
 * @param addr local addrinfo für den socket
 *
 * @return der server socket
 */
int init_socket(struct addrinfo* addr);

/**
 * @brief konfiguriert einen socket zu non-blocking mit #fnnctl()
 *
 * @param sockfd der socket
 *
 * @return false falls kritische fehler auftreten
 */
bool set_nonblocking(int sockfd);

/**
 * @brief akzeptiert eingehende Verbindungen
 *
 * @param der server socket
 * @param pointer zu result memory
 *
 * @return  0 -> no client to accept
 *          1 -> client returned
 *         -1 -> error unwinding
 */
int accept_client(int fd, struct client* c);

/**
 * @brief überprüft alle client sockets auf neue messages
 *
 * @param fds      pollfd array mit poll results
 * @param clients  client* array mit allen verbundenen clients
 * @param ccount   anzahl der verbundenen clients
 * @param mask     pointer zu allocated bitmask
 *
 * @return false -> kritische fehler sind aufgetreten, stoppe server
 */
bool handle_client_poll(struct pollfd fds[], struct client *clients[MAX_CONN], int *ccount, bool *mask);

/**
 * @brief überprüft den server socket auf neue client Verbindungen
 *
 * @param spollfds  pollfd mit poll result von dem server socket
 * @param clients   client* array mit allen verbundenen clients
 * @param ccount    anzahl der verbundenen clients
 *
 * @return false -> kritische fehler sind aufgetreten, stoppe server
 */
bool handle_socket_poll(struct pollfd spollfd, struct client *clients[MAX_CONN], int *ccount);

#endif // NETWORKING_H
