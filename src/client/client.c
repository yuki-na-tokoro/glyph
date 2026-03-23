#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <poll.h>
#include <ncurses.h>

#include "client.h"
#include "../protocol.h"
#include "commands.h"
#include "screen.h"

int main(void){

    // int sockfd = connect_to_server(HOST, PORT);   // Socket eröffnen und mit Server verbinden

    poll_loop();  // Interaktion mit dem Server

    // endwin();
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

int connect_to_server(char *host, char *port){

    // Beej Abschnitt 5.1
    int status;
    struct addrinfo hints;  // Parameter für getaddrinfo()
    struct addrinfo *servinfo;  // Ausgabe von getaddrinfo()
    int sockfd;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;        // IP Format irrelevant
    hints.ai_socktype = SOCK_STREAM;    // TCP (Stream) Socket

    if ((status = getaddrinfo(host, port, &hints, &servinfo)) != 0) { // Fehlerbehandlung für getaddrinfo()
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



/*
TODO:   - Terminal User Interface für den Client mithilfe von ncurses implementieren (Mby Histroy implementieren)
        - Introduce, Channel etc. in commands.c integrieren
        - channelnamen und nickname an send_command übergeben für Commands die dies brauchen
        - Feature das wenn man eine Nachricht eingibt und einen Log von vergangenen Eingaben behält,
          damit man bspw. Pfeil hoch drücken kann um die letzt Eingabe zu wiederholen
        - channelnamen bei Änderung aktualisieren, z.B. bei /part
*/

void poll_loop(){

    char *host;
    char *port = "6667"; // Default Port, wird geändert insofern der Nutzer einen Port als Argument angibt
    char *nickname;
    if(getenv("USER") != NULL){
        nickname = strdup(getenv("USER"));
    } 
    else{
        nickname = strdup("guest");
    }
    char *channelname;

    int sockfd = -1;

    // Beej Abschnitt 7.2
    struct pollfd pfds[2];
    pfds[0].fd = 0; // Ausgehende Nachrichten
    pfds[0].events = POLLIN;

    pfds[1].fd = sockfd;    // Eingehende Nachrichten
    pfds[1].events = POLLIN;
    
    //int step = 1;

    while (true){
        poll(pfds, 2, -1);
        char answer[MSG_SIZE];
        // char input[MSG_SIZE];

        /*
        if(step == 0){ // Host vom Nutzer holen (irc.libera.chat)
            nickname = introduce(sockfd);
            // setupscreen();
            step++;
        }
        if(step == 1){
            nickname = introduce(sockfd);
            // setupscreen();
            step++;
        }
        if(step == 2){
            channelname = channel(sockfd);
            step++;
        }
            */

        if(pfds[0].revents & POLLIN){
            // get_eingabe(input);
            char *input = get_input();
            if(input == NULL){ //Nachricht vom Nutzer einlesen
                // chat_print("Fehler beim Einlesen.");
                printf("Fehler beim Einlesen.");
                continue;
            }
            if(strncmp(input, "/quit", 5) == 0 || strncmp(input, "/q", 2) == 0){    // Programm mit /quit oder /q schließen
                send_msg(sockfd, "QUIT\r\n");
                free(input);
                channelname = NULL;
                break;
            }
            send_command(&sockfd, input, &host, &port, &nickname, &channelname);
            pfds[1].fd = sockfd;
        }

        if(pfds[1].revents & POLLIN){
            recv_msg(sockfd, answer, MSG_SIZE);   // Nachricht vom Server erhalten
            if(strncmp(answer, "PING", 4) == 0){
                send_msg(sockfd, "PONG");
            }

            //chat_print(answer);

            print_answer(answer, nickname);
        }
    }

    free(channelname);
    free(nickname);
    close(sockfd);
}


char *introduce(int sockfd){
    char *nickname = malloc(11);
    char nickmsg[MSG_SIZE];
    char usermsg[MSG_SIZE];
    while(true){
        printf("Nickname eingeben (Maximal 9 Zeichen): ");
        
        if(fgets(nickname, 11, stdin) == NULL){
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

        char *format = "NICK %s\r\n";

        if(strlen(nickname) >= 1){
            snprintf(nickmsg, sizeof(nickmsg), format, nickname);    // Nickname korrekt formatieren
            snprintf(usermsg, sizeof(usermsg), "USER guest 0 * : %s\r\n", nickname);    // USER Message korrekt formatieren
            break;
        }
        else{
            printf("Kein Nickname eingegeben.\n");
            continue;
        }
    }

    // NICK
    send_msg(sockfd, nickmsg);

    // USER
    send_msg(sockfd, usermsg);

    return nickname;
}

char *channel(int sockfd){
    char *channelname= malloc(202);
    char channelmsg[MSG_SIZE];

    while(true){
        printf("Channelname: ");
        
        if(fgets(channelname, 202, stdin) == NULL){
            printf("Fehler beim Lesen.\n");
            continue;
        }


        if(strchr(channelname, '\n') == NULL){ // Falls der Channelname von fgets() abgeschnitten wurde
            int c;
            printf("Channelname zu lang.\n");
            
            while((c = getchar()) != '\n' && c != EOF); // Buffer für erneute Eingabe leeren

            continue;
        }

        channelname[strcspn(channelname, "\n")] = '\0';   // Ersetzt Newline mit Stringende

        if(strlen(channelname) >= 1){
            snprintf(channelmsg, sizeof(channelmsg), "JOIN %s\r\n", channelname);    // Channelname korrekt formatieren
            break;
        }
        else{
            printf("Kein Channelname eingegeben.\n");
            continue;
        }
    }

    // JOIN
    send_msg(sockfd, channelmsg);

    return channelname;
}


/*
TODO:   Nachrichten kommen in answer teilweise nicht als einzelne Zeilen sondern als ein großter Block im Buffer. 
        Dementsprechend muss der Buffer am Zeilenende aufgeteilt werden um die einzelnen Zeilen zu erhalten und 
        entsprechend zu formatieren. Vermutlich mit Schleife die jeden String bis zu einem Newline entsprechend
        formatiert und printet. Schleife endet dann am EOF. Zudem noch irrelevante Nachrichten rausfiltern. 

        Funktion ist in der "Chat-Phase" nicht mehr funktional weil Nachrichten dann den nickname nicht mehr
        beinhalten wodurch einfach jede Nachricht gefilte
        rt wird.

        -> Auch noch entsprechende print_msg bei der ausgehende Nachrichten formatiert ausgegeben werden
*/

void print_answer(char *answer, char *nickname){

    if(nickname == NULL){
        printf("%s\n", answer);
        return;
    }


    char *dupl = strdup(answer);    // macht ne kopie vom string damit "echter" string nicht kaput gemacht wird
    char *line = strtok(dupl, "\n");    //spaltet string an zeilen ende auf 
    while (line != NULL){
        char *posnick = strstr(line, nickname); //sucht position vom Nickname
        char *poscol = (posnick != NULL) ? strchr(posnick, ':') : NULL; //sucht poition vom Doppelpunkt nach Nickname

        if(posnick == NULL || poscol == NULL){
            // chat_print(line);    // wenn kein nickname oder kein doppelpunkt gefunden: unveränderte zeile wird geprintet
            printf("%s\n", line);

        }else{
            // chat_print((poscol + 1));
            printf("%s\n", (poscol + 1)); // zeilen inhalt nach doppelpunkt geprintet
        }

        line = strtok(NULL, "\n");  //strtok macht in nächster zeile weiter
    }
    free(dupl); 

}


