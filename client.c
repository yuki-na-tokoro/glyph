#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <poll.h>

#include "client.h"
#include "protocol.h"

void printanswer(char *answer, char *nickname){


    char *posnick = strstr(answer, nickname); //  sucht position vom Nickname in der Antwort
    char *poscol = strchr(posnick, ":");
    if (posnick != NULL && poscol != NULL){
         printf("%s\n" poscol);
    }


}

int main(void){
    int sockfd = connect_to_server();   // Socket eröffnen und mit Server verbinden

    poll_loop(sockfd);  // Interaktion mit dem Server

    close(sockfd);
}

void send_msg(int sockfd, char *msg){
    int len = strlen(msg);  // Länge der MSG
    send(sockfd, msg, len, 0); // Nachricht versenden
}

void recv_msg(int sockfd, char *msg, int buffersize){
    int count = recv(sockfd, msg, buffersize - 1, 0);   // recv() gibt die Menge der empfangenen Zeichen zurück
    if(count <= 0){
        perror("Connection lost / closed");
        exit(1);
    }

    msg[count] = '\0';  // String abschließen
}

int connect_to_server(){
    // Beej Abschnitt 5.1
    int status;
    struct addrinfo hints;  // Parameter für getaddrinfo()
    struct addrinfo *servinfo;  // Ausgabe von getaddrinfo()
    int sockfd;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;        // IP Format irrelevant
    hints.ai_socktype = SOCK_STREAM;    // TCP (Stream) Socket

    if ((status = getaddrinfo(HOST, PORT, &hints, &servinfo)) != 0) { // Fehlerbehandlung für getaddrinfo()
        fprintf(stderr, "gai error : %s\n", gai_strerror(status));
        exit(1);
    }
    
    // Beej Abschnitt 5.2
    sockfd = socket(servinfo -> ai_family, servinfo -> ai_socktype, servinfo -> ai_protocol);   // File Descriptor für den Socket erstellen
    if (sockfd == -1) {
        perror("local socket error");
        exit(1);
    }

    // Beej Abschnitt 5.4
    if (connect(sockfd, servinfo -> ai_addr, servinfo -> ai_addrlen) == -1) {   // Über Socket mir Server verbinden
       perror("socket connection error");
       exit(1);
    }

    freeaddrinfo(servinfo);

    return sockfd;
}

void poll_loop(int sockfd){
    int step = 1;

    // Beej Abschnitt 7.2
    struct pollfd pfds[2];
    pfds[0].fd = 0; // Ausgehende Nachrichten
    pfds[0].events = POLLIN;

    pfds[1].fd = sockfd;    // Eingehende Nachrichten
    pfds[1].events = POLLIN;

    while (true){
        poll(pfds, 2, -1);
        char input[MSG_SIZE];
        char answer[MSG_SIZE];

        if(step == 1){
            introduce(sockfd);
            step++;
        }
        if(step == 2){
            // TODO
        }
        if(pfds[0].revents & POLLIN){
            if(fgets(input, sizeof(input), stdin) == NULL){ //Nachricht vom Nutzer einlesen
                printf("Fehler beim Einlesen.");
                continue;
            }
            send_msg(sockfd, input);    
        }

        if(pfds[1].revents & POLLIN){
            recv_msg(sockfd, answer, MSG_SIZE);   // Nachricht vom Server erhalten
            if(strncmp(answer, "PING", 4) == 0){
                send_msg(sockfd, "PONG");
            }
            printf("%s", answer);
        }
    }
}

void introduce(int sockfd){
    char nickname[11];
    char nickmsg[MSG_SIZEchar nickname[11];];
    char usermsg[MSG_SIZE];
    while(true){
        printf("Nickname eingeben (Maximal 9 Zeichen): ");
        
        if(fgets(nickname, sizeof(nickname), stdin) == NULL){
        printf("Fehler beim Lesen.\n");
        continue;
        }


        if(strchr(nickname, '\n') == NULL){ // Falls der Nick von fgets() abgeschnitten wurde
            int c;
            printf("Nickname zu lang.\n");
            
            while((c = getchar()) != '\n' && c != EOF); // Buffer für erneute Eingabe leeren

            continue;
        }

        nickname[strcspn(nickname, "\n")] = '\0';   // Ersetzt Newline mit Stringende

        if(strlen(nickname) >= 1){
            snprintf(nickmsg, sizeof(nickmsg), "NICK %s\r\n", nickname);    // Nickname korrekt formatieren
            snprintf(usermsg, sizeof(usermsg), "USER guest 0 * : %s\r\n", nickname);    // USER Message korrekt formatieren
            break;
        }
        else{
            printf("Kein Nickname eingegeben.\n");
            continue;
        }
    }
   
    char *answer = malloc(MSG_SIZE);

    // NICK
    send_msg(sockfd, nickmsg);

    // USER
    send_msg(sockfd, usermsg);

    free(answer);
}
<<<<<<< HEAD

void poll_loop(int sockfd){
    int step = 1;

    // Beej Abschnitt 7.2
    struct pollfd pfds[2];
    pfds[0].fd = 0;
    pfds[0].events = POLLIN;

    pfds[1].fd = sockfd;
    pfds[1].events = POLLIN;

    while (true){
        int count = poll(pfds, 2, 2500);    // Anzahl an einkommenden bzw. ausgehenden Nachrichten
        char input[MSG_SIZE];
        char answer[MSG_SIZE];

        if(count == 0){
            printf("Poll timed out");
            continue;
        }
        else{
            if(step == 1){
                introduce(sockfd);
                step++;
            }
            if(pfds[0].revents & POLLIN){
                if(fgets(input, sizeof(input), stdin) == NULL){ //Nachricht vom Nutzer einlesen
                    printf("Fehler beim Einlesen.");
                    continue;
                }
                send_msg(sockfd, input);    
            }

            if(pfds[1].revents & POLLIN){
                recv_msg(sockfd, answer, MSG_SIZE);   // Nachricht vom Server erhalten

                printf("%s", answer);
            }
        }
    }
}

int main(void){
    int sockfd = connect_to_server();

    poll_loop(sockfd);

    close(sockfd);
}
=======
>>>>>>> 202f328ea6bcc6eb6a0bdc92966bbb3282389f62
