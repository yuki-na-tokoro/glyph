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
    dprintf(sockfd, "%s", msg); // Nachricht versenden
}

int recv_msg(int sockfd, char *msg, int buffersize){
    int count = recv(sockfd, msg, buffersize - 1, 0);   // recv() gibt die Menge der empfangenen Zeichen zurück
    if(count <= 0){
        chat_print("Verbindung geschlossen/verloren");
        return -1;
    }

    msg[count] = '\0';  // String abschließen

    return 0;
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
        chat_print("Konnte keine Verbindung zum Server herstellen.");
        return -1;
    }
    
    // Beej Abschnitt 5.2
    sockfd = socket(servinfo -> ai_family, servinfo -> ai_socktype, servinfo -> ai_protocol);   // File Descriptor für den Socket erstellen
    if (sockfd == -1) {
        chat_print("Konnte keinen Socket erstellen.");
        return -1;
    }

    // Beej Abschnitt 5.4
    if (connect(sockfd, servinfo -> ai_addr, servinfo -> ai_addrlen) == -1) {   // Über Socket mir Server verbinden
       chat_print("Konnte den Socket nicht verbinden.");
       return -1;
    }

    freeaddrinfo(servinfo);

    return sockfd;
}



/*
TODO:    (Mby Histroy implementieren)
        
        - Feature das wenn man eine Nachricht eingibt und einen Log von vergangenen Eingaben behält,
          damit man bspw. Pfeil hoch drücken kann um die letzt Eingabe zu wiederholen
          am besten neuer ifelse fall für pfeil hoch zwischen den backspace und den normale zeichen fall
          der den tipp_buf auf den letzten command setzt.
*/

void poll_loop(){

    char *host = NULL;
    char *port = "6667"; // Default Port, wird geändert insofern der Nutzer einen Port als Argument angibt
    char *nickname;
    if(getenv("USER") != NULL){
        nickname = strdup(getenv("USER"));
    } 
    else{
        nickname = strdup("guest");
    }
    char *channelname = NULL;

    int sockfd = -1;
    setupscreen();

    char tipp_buf[MSG_SIZE];
    memset(tipp_buf, 0, MSG_SIZE);
    int tbuf_index = 0;

    // Beej Abschnitt 7.2
    struct pollfd pfds[1];
    pfds[0].fd = sockfd;
    pfds[0].events = POLLIN;
    wmove(infos, 0, 0);
    wclrtoeol(infos);
    if(host==NULL){
        wprintw(infos,"[ZEIT]   [%s]   [kein hostname]", nickname);
    }else{
        wprintw(infos,"[ZEIT]   [%s]   [%s]", nickname, host);
    }
    wrefresh(infos);
    wtimeout(eingabefeld, 0);

    while (true){  
        
        poll(pfds, 1, 50);

        if(sockfd != -1 && (pfds[0].revents & POLLIN)){
            char answer[MSG_SIZE];
            if(recv_msg(sockfd, answer, MSG_SIZE) == -1){   // Falls der Server die Verbindung schließt wird der Socket zurückgesetzt und der Nutzer kann sich neu verbinden
                close(sockfd);
                sockfd = -1;
                pfds[0].fd = -1;
                chat_print("Verbindung zum Server geschlossen. Nutze /connect um dich erneut zu verbinden.");
                continue;
            } 
            else if(strncmp(answer, "PING", 4) == 0){
                send_msg(sockfd, "PONG");
            }
            print_answer(answer, nickname);
            //doupdate();
        }
        int ch = wgetch(eingabefeld);

        if (ch != ERR){
            if (ch == '\n' || ch == KEY_ENTER){ //wenn enter gedrückt wird wird buffer verarbeitet
                if (tbuf_index > 0){    //prüft ob eingabe vorhanden
                    if (strcmp(tipp_buf, "/quit") == 0 || strcmp(tipp_buf, "/q") == 0){ //prüft auf quit befehl -> quitet
                        break;
                    }


                chat_print(tipp_buf);   //eigabe wird auf screen geprintet
                send_command(&sockfd, tipp_buf, &host, &port, &nickname, &channelname); //command wird gesendet
                pfds[0].fd = sockfd;
                memset(tipp_buf, 0, MSG_SIZE);  //tipbuf wird auf null gesetzt
                tbuf_index = 0; 

                
                }
            }else if (ch == KEY_BACKSPACE){ //wenn backspace gedrückt wird, dann wird letzter eingegebener character aus eingabe (tipbuf) gelöscht
                if(tbuf_index >0){
                tipp_buf[--tbuf_index] = '\0';
               
                }

            }else if (ch >= 32 && ch <= 126 && tbuf_index < MSG_SIZE - 1){  //wenn normale zecichen kommen werden sie an tippbuf angehängt 
                tipp_buf[tbuf_index] = (char)ch;
                tbuf_index++;
                tipp_buf[tbuf_index] ='\0';
            }

        }
        //eingabe wird im eingabefeld angezeigt screen wird refreshed
        wmove(infos, 0, 0);
        wclrtoeol(infos);
        wprintw(infos,"[ZEIT]   [%s]   [%s]", nickname, host);
        wmove(eingabefeld, 1, 1);
        wclrtoeol(eingabefeld);
        if(channelname == NULL){
            mvwprintw(eingabefeld, 1, 1,"[no channel] > %s", tipp_buf);
        }else{
             mvwprintw(eingabefeld, 1, 1,"[%s] > %s",channelname, tipp_buf);
        }
       
        box(eingabefeld, 0, 0);
        
        wnoutrefresh(eingabefeld);
        doupdate();
        
    } 


    endwin();
    // free(port);
    free(host);
    free(channelname);
    free(nickname);
    close(sockfd);
}


/*
TODO:   - Wenn strtok verwendet wird schreibt das in den Befehlsbuffer d.h. wenn eine Nachricht wie PING vom Server ankommt dann wird der
          nächste Befehl nicht erkannt bzw. falsch erkannt (Zu fixen durch strtok_r() ?)
*/

void print_answer(char *answer, char *nickname){

    
    if(nickname == NULL){
        char *copy = strdup(answer);
        char *line2 = strtok(copy, "\r\n");
        while(line2 != NULL){
            chat_print("%s", line2);
            line2 = strtok(NULL, "\r\n");
        }
        free(copy);
        return;
    }

    char *dupl = strdup(answer);    // macht ne kopie vom string damit "echter" string nicht kaput gemacht wird
    char *line = strtok(dupl, "\r\n");    //spaltet string an zeilen ende auf
    while (line != NULL){
        if(strlen(line) > 0){
            char *posnick = strstr(line, nickname); //sucht position vom Nickname
            char *poscol = (posnick != NULL) ? strchr(posnick, ':') : NULL; //sucht position vom Doppelpunkt nach Nickname

            if(posnick == NULL || poscol == NULL){
                chat_print("%s", line);
            }else{
                chat_print("%s", poscol + 1); // zeilen inhalt nach doppelpunkt geprintet
            }
        }
        line = strtok(NULL, "\r\n");  //strtok macht in nächster zeile weiter
    }
    free(dupl); 

}


