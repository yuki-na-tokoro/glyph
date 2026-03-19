#ifndef NETWORKING_H
#define NETWORKING_H

#include "client.h"

#define BACKLOG 10
#define MAX_CONN 128
#define TIMEOUT 3 * 60 * 1000

int init_socket(struct addrinfo*);

void poll_listening(int, struct client* clients[MAX_CONN], int* ccount);

struct client* accept_client(int fd);

#endif // NETWORKING_H
