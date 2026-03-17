#include <netinet/in.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

#include "client.h"
#include "protocol.h"

void *send_and_receive(char* msg, int sockfd, char *answer){
    int len = strlen(msg);  // Länge der MSG
    send(sockfd, msg, len, 0); // Nachricht versenden

    sleep(1);

    if (recv(sockfd, answer, 60000000, 0) <= 0) { // Auf Antwort warten
        perror("connection closed / failed");   // Error Handling falls Fehlercode
        exit(1);
    }

    return answer;
}



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
    
    sockfd = socket(res -> ai_family, res -> ai_socktype, res -> ai_protocol);
    if (sockfd == -1) {
        perror("local socket error");
        exit(1);
    }

    if (connect(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
       perror("server socket connect error");
       exit(1);
    } else {
        printf("server socket connected succesfully\n");
    };

    
    char *answer = malloc(128);
    char nickname[9];
    char nickmsg[32];
    while(true){
        printf("Nickname eingeben (Maximal 8 Zeichen): ");
        fgets(nickname, sizeof(nickname), stdin);
        nickname[strcspn(nickname, "\n")] = '\0';
        snprintf(nickmsg, sizeof(nickmsg), "NICK %s\r\n", nickname);
        if(strlen(nickname) >= 1 && strlen(nickname) <= 9){
            break;
        }
        else{
            printf("Nickname zu lang bzw. kurz\n");
        }
    }

    send_and_receive("USER guest artemis servername : Artemis \r\n", sockfd, answer);
    printf("%s", answer);

    send_and_receive(nickmsg, sockfd, answer);
    printf("%s", answer);

    send_and_receive("TIME \r\n", sockfd, answer);
    printf("%s", answer);

    free(answer);
    close(sockfd);
    freeaddrinfo(servinfo);
}
