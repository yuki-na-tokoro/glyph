#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include "client.h"

int main(void){

    // Beej Abschnitt 5.1
    int status;
    struct addrinfo hints;  // Parameter für getaddrinfo()
    struct addrinfo *servinfo;  // Ausgabe von getaddrinfo()
    struct addrinfo *res; // local
    int sockfd;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;        // IP Format irrelevant
    hints.ai_socktype = SOCK_STREAM;    // TCP (Stream) Socket

    if ((status = getaddrinfo(HOST, PORT, &hints, &servinfo)) != 0) { // Fehlerbehandlung für getaddrinfo()
        fprintf(stderr, "gai error : %s\n", gai_strerror(status));
        exit(1);
    }

    hints.ai_flags = AI_PASSIVE;

    if ((status = getaddrinfo(NULL, PORT, &hints, &res)) != 0) { // Fehlerbehandlung für getaddrinfo()
        fprintf(stderr, "gai error : %s\n", gai_strerror(status));
        exit(1);
    }
    
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd == -1) {
        perror("socket error");
        exit(1);
    }

    if (connect(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
       perror("connect error");
       exit(1);
    } else {
        printf("connected succesfully\n");
    };

    char* msg;
    int len, bytes_sent;
    char answer[64];

    msg = "NICK artemis";
    len = strlen(msg);
    bytes_sent = send(sockfd, msg, len, 0);

    if (recv(sockfd, answer, 64, 0) <= 0) {
        perror("connection closed / failed");
        exit(1);
    }
    printf("%s\n", answer);

    msg = "USER guest artemis servername :Artemis";
    len = strlen(msg);
    bytes_sent = send(sockfd, msg, len, 0);

    if (recv(sockfd, answer, 64, 0) <= 0) {
        perror("connection closed / failed");
        exit(1);
    }
    printf("%s\n", answer);
       
    msg = "TIME";
    len = strlen(msg);
    bytes_sent = send(sockfd, msg, len, 0);

    if (recv(sockfd, answer, 64, 0) <= 0) {
        perror("connection closed / failed");
        exit(1);
    }
    printf("%s\n", answer);
       
    close(sockfd);
    freeaddrinfo(servinfo);
}
