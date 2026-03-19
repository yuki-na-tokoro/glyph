#ifndef NETWORKING_H
#define NETWORKING_H

#include "client.h"

#define BACKLOG 10
#define MAX_CONN 128
#define TIMEOUT 3 * 60 * 1000

int init_socket(struct addrinfo*);

void poll_listening(int fd, struct client* clients[MAX_CONN], int* ccount);

struct client* accept_client(int fd);

void poll_clients(struct client* clients[MAX_CONN], int* ccount);

#endif // NETWORKING_H
