#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>

#define HOST "irc.libera.chat"
#define PORT "6667"



int main(void){

    // Beej Abschnitt 5.1
    int status;
    struct addrinfo hints;  // Parameter für getaddrinfo()
    struct addrinfo *servinfo;  // Ausgabe von getaddrinfo()

    memset(&hints, 0, sizeof hints);    
    hints.ai_family = AF_UNSPEC;        // IP Format irrelevant
    hints.ai_socktype = SOCK_STREAM;    // TCP (Stream) Socket

    if((status = getaddrinfo(HOST, PORT, &hints, &servinfo)) != 0){ // Fehlerbehandlung für getaddrinfo()
        fprintf(stderr, "gai error : %s\n", gai_strerror(status));
        exit(1);
    }

    freeaddrinfo(servinfo);
}