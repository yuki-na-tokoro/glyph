#ifndef NETWORKING_H
#define NETWORKING_H

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
 */
void set_nonblocking(int sockfd);

/**
 * @brief akzeptiert eingehende Verbindungen
 *
 * @param der server socket
 *
 * @return client struct mit socket, addrinfo, usw.
 */
struct client* accept_client(int fd);

/**
 * @brief überprüft alle client sockets auf neue messages
 *
 * @param fds      pollfd array mit poll results
 * @param clients  client* array mit allen verbundenen clients
 * @param ccount   anzahl der verbundenen clients
 *
 * @return bitmask für geschlossene sockets
 */
bool* handle_client_poll(struct pollfd fds[], struct client *clients[MAX_CONN], int *ccount);

/**
 * @brief überprüft den server socket auf neue client Verbindungen
 *
 * @param spollfds  pollfd mit poll result von dem server socket
 * @param clients   client* array mit allen verbundenen clients
 * @param ccount    anzahl der verbundenen clients
 */
void handle_socket_poll(struct pollfd spollfd, struct client *clients[MAX_CONN], int *ccount);

#endif // NETWORKING_H
