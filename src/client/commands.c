#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ncurses.h>

#include "client.h"
#include "../protocol.h"
#include "commands.h"
#include "screen.h"

struct command commands[CLIENT_COMMANDS] = {
    {"Nickname", "/nick",   "NICK %s\r\n",            false,    17,       1}, //
    {"User",     "/user",   "USER %s 0 * :%s\r\n",    false,    MSG_SIZE, 2}, //
    {"Connect",  "/connect","CONNECT %s 6667\r\n",    false,    MSG_SIZE, 1}, //
    {"Quit",     "/quit",   "QUIT\r\n",               false,    MSG_SIZE, 0}, // Memory Leak ?
    {"Passwort", "/pass",   "PASS %s\r\n",            false,    MSG_SIZE, 1}, // Idk wie ich das testen soll
    {"Join",     "/join",   "JOIN %s\r\n",            false,    MSG_SIZE, 1}, // 
    {"Part",     "/part",   "PART %s\r\n",            true,     MSG_SIZE, 1}, //
    {"Topic",    "/topic",  "TOPIC %s\r\n",           true,     MSG_SIZE, 1}, // Kann noch kein Topic setzen
    {"Names",    "/names",  "NAMES %s\r\n",           true,     MSG_SIZE, 1}, // 
    {"List",     "/list",   "LIST\r\n",               false,    MSG_SIZE, 0}, //
    {"Kick",     "/kick",   "KICK %s %s :%s\r\n",     false,    MSG_SIZE, 3}, //
    {"Invite",   "/invite", "INVITE %s %s\r\n",       false,    MSG_SIZE, 2}, //
    {"Mode",     "/mode",   "MODE %s %s\r\n",         false,    MSG_SIZE, 2}, //
    {"Msg",      "/msg",    "PRIVMSG %s :%s\r\n",     false,    MSG_SIZE, 2}, //
    {"Notice",   "/notice", "NOTICE %s :%s\r\n",      false,    MSG_SIZE, 2}, //
    {"Whois",    "/whois",  "WHOIS %s\r\n",           true,     MSG_SIZE, 1}, //
    {"Who",      "/who",    "WHO %s\r\n",             false,    MSG_SIZE, 1}, //
    {"Motd",     "/motd",   "MOTD\r\n",               false,    MSG_SIZE, 0}, //
};

/*
TODO:   - Sicherheitschecks und Errohandling überprüfen und einbauen
        - Refactoren weil dieser Shit ist hella unübersichtlich
        - Handeln wenn Nickname bereits vergeben

*/

int send_command(int *sockfd, char *input, char **host, char **port, char **nickname, char **channelname){
    int count = 0;

    if(*sockfd == -1 && strncmp(input, "/connect", 8) != 0){
            chat_print("Nicht verbunden. Nutze /connect um dich mit einem Server zu verbinden");    // Erlaubt keine anderen Commands wenn noch nicht mit Host verbunden
            return -1;
        }

    
    if(strncmp(input, "/", 1) != 0){    // Für normale MSG damit man da Command und Channel nicht vorschreiben muss
        if(*channelname == NULL){
            chat_print("Keinem Channel beigetreten");   // Dies wird fehlschlagen wenn man von einem Channel parted weil es nicht aktualisiert wird
            return -1;
        }

        char msg[MSG_SIZE];
        char *comm = "/msg";    // Command für den Benutzer setzen
        int i = what_command(comm); // Stelle von /msg finden. Sicher für zukünftige Änderungen an commands

        count = snprintf(msg, commands[i].size, commands[i].format, *channelname, input);  // Nachricht in das Serverformat bringen

        if(count >= commands[i].size){
            chat_print("Eingabe zu lang");
            return -1;
        }
        send_msg(*sockfd, msg);
        
        return 0;    // Hier noch einfügen das die Message geprinted wird
    }
    else{

        char *comm = strtok(input, " ");    // Command aus dem Input nehmen
        if(comm == NULL){
            chat_print("Eingabe kein Command");
            return -1;
        }

        int i = what_command(comm);
        if(i == -1){
            chat_print("Unbekannter Command");  // Command nicht im commands struct
            return -1;
        }

        char msg[commands[i].size];

        switch (commands[i].argc){
            case 0: {   // Keine Argumente übergeben
                strcpy(msg, commands[i].format);    // Nachricht in das Serverformat bringen
                send_msg(*sockfd, msg);
                
                return 0;
            }
            case 1: {   // Ein Argument übergeben
                char *arg1 = strtok(NULL, " ");

                if(arg1 == NULL && commands[i].channel != true){    // keine Argument gegeben und Channel kann nicht automatisch übergeben werden
                    chat_print("Zu wenige Argumente übergeben");
                    return -1;
                }
                else if(commands[i].channel == true){
                    if(*channelname == NULL){
                        chat_print("Keinem Channel beigetreten");
                        return -1;
                    }
                    arg1 = *channelname;    // aktuellen Channel als Argument für den Nutzer einsetzen
                }

                count = snprintf(msg, commands[i].size, commands[i].format, arg1);  // Nachricht in das Serverformat bringen

                if(count >= commands[i].size){
                    chat_print("Eingabe zu lang");
                    return -1;
                }
                
                if(strcmp(comm, "/connect") == 0){
                    free(*host);
                    *host = strdup(arg1);   // Host neu zurordnen falls reconnection

                    *sockfd = connect_to_server(*host, *port);  // Verbindung zum Server herstellen

                    if(*sockfd == -1) return -1;

                    char usermsg[MSG_SIZE];
                    count = snprintf(usermsg, sizeof(usermsg), "USER %s 0 * :%s\r\n", *nickname, *nickname);

                    if(count >= MSG_SIZE){
                        chat_print("Nickname zu lang");
                        return -1;
                    }

                    send_msg(*sockfd, usermsg); // Username automatisch auf Systembenutzername setzen

                    char nickmsg[MSG_SIZE];

                    count = snprintf(nickmsg, sizeof(nickmsg), "NICK %s\r\n", *nickname);

                    if(count >= MSG_SIZE){
                        chat_print("Nickname zu lang");
                        return -1;
                    }

                    send_msg(*sockfd, nickmsg); // Nicknam automatisch auf Systembenutzername setzen

                    return 0;
                }
                else if(strcmp(comm, "/nick") == 0){
                    free(*nickname);
                    *nickname = strdup(arg1);   // Nickname aktualisieren
                }
                else if(strcmp(comm, "/join") == 0){
                    free(*channelname);
                    *channelname = strdup(arg1);    // Nickname aktualisieren
                }
                else if(strcmp(comm, "/part") == 0){
                    free(*channelname);
                    *channelname = NULL;    // Channelname aktualisieren wenn channel verlassen wird
                }

                send_msg(*sockfd, msg);
                
                return 0;
            }
            case 2: {   // Zwei Argumente übergeben
                char *arg1 = NULL;
                char *arg2;
                if(strcmp(comm, "/msg") == 0 || strcmp(comm, "/notice") == 0){  // Falls PRIVMSG oder NOTICE weil die brauchen mehr Space für die Nachricht
                    arg1 = strtok(NULL, " ");   //  Liest den Channel 
                    arg2 = strtok(NULL, "");    // Liest den Rest der Nachricht
                    if(arg2 == NULL){
                        chat_print("Zu wenig Argumente übergeben");
                        return -1;
                    }
                }
                else{   // Normales Handling für jeden anderen Command
                    arg1 = strtok(NULL, " ");
                    arg2 = strtok(NULL, " ");
                }

                if(strcmp(comm, "/invite") == 0 && arg1 != NULL && arg2 == NULL){   // Channelname für den User setzen
                    arg2 = *channelname;
                }
                if(strcmp(comm, "/mode") == 0 && arg1 != NULL && arg2 == NULL){ // Channelname für den User setzen
                    arg2 = arg1;
                    arg1 = *channelname;
                }
                else if(arg1 == NULL || arg2 == NULL){
                    chat_print("Zu wenige Argumente übergeben");
                    
                    return -1;
                }

                count = snprintf(msg, commands[i].size, commands[i].format, arg1, arg2);    // Nachricht in das Serverformat bringen

                if(count >= commands[i].size){
                    chat_print("Eingabe zu lang");
                    
                    return -1;
                }

                send_msg(*sockfd, msg);
                
                return 0;
            }
            case 3: {   // Drei Argumente übergeben
                char *arg1 = strtok(NULL, " ");
                char *arg2 = strtok(NULL, " ");
                char *arg3 = strtok(NULL, " ");

                if(strcmp(comm, "/kick") == 0 && arg1 != NULL && arg2 != NULL && arg3 == NULL){ // Channelnamen für den User setzen
                    arg3 = arg2;
                    arg2 = arg1;
                    arg1 = *channelname;
                }

                else if(arg1 == NULL || arg2 == NULL || arg3 == NULL){
                    chat_print("Zu wenige Argumente übergeben");
                    
                    return -1;
                }

                count = snprintf(msg, commands[i].size, commands[i].format, arg1, arg2, arg3);  // Nachricht in das Serverformat bringen

                if(count >= commands[i].size){
                    chat_print("Eingabe zu lang");
                    
                    return -1;
                }

                send_msg(*sockfd, msg);
                
                return 0;
            }
            default:{   // Idk wie bzw. wann dieser Fall eintreten sollte
                chat_print("Error sending message arguments");
                return -1;
            }
        }
    }   
}


int what_command(char *input){
    for(int i = 0; i < CLIENT_COMMANDS; i++){   // Iteriert über Commands um den gesuchten zu finden
        if(strcmp(commands[i].comm, input) == 0){
            return i;
        }
    }

    return -1;
}
