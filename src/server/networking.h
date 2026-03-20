#ifndef NETWORKING_H
#define NETWORKING_H

#include "client.h"

#define BACKLOG 10
#define MAX_CONN 128
#define TIMEOUT 3 * 60 * 1000

int init_socket(struct addrinfo*);

void set_nonblocking(int sockfd);

struct client* accept_client(int fd);

bool* handle_client_poll(struct pollfd fds[], struct client *clients[MAX_CONN], int *ccount);

void handle_socket_poll(struct pollfd spollfd, struct client *clients[MAX_CONN], int *ccount);

#endif // NETWORKING_H
