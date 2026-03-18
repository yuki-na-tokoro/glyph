#ifndef NETWORKING_H
#define NETWORKING_H

#include <netdb.h>

#define BACKLOG 10
#define MAX_CONN 128


int init_socket(struct addrinfo*);

#endif // NETWORKING_H
