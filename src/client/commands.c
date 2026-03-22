#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include "client.h"
#include "../protocol.h"
#include "commands.h"

struct command commands[CLIENT_COMMANDS] = {
    {"Nickname", "/nick",   "NICK %s\r\n",           17,       1},
    {"User",     "/user",   "USER %s 0 * :%s\r\n",   MSG_SIZE, 2},
    {"Quit",     "/quit",   "QUIT\r\n",               MSG_SIZE, 0},
    {"Passwort", "/pass",   "PASS %s\r\n",            MSG_SIZE, 1},
    {"Join",     "/join",   "JOIN %s\r\n",            MSG_SIZE, 1},
    {"Part",     "/part",   "PART %s\r\n",            MSG_SIZE, 1},
    {"Topic",    "/topic",  "TOPIC %s\r\n",           MSG_SIZE, 1},
    {"Names",    "/names",  "NAMES %s\r\n",           MSG_SIZE, 1},
    {"List",     "/list",   "LIST\r\n",               MSG_SIZE, 0},
    {"Kick",     "/kick",   "KICK %s %s :%s\r\n",     MSG_SIZE, 3},
    {"Invite",   "/invite", "INVITE %s %s\r\n",       MSG_SIZE, 2},
    {"Mode",     "/mode",   "MODE %s %s\r\n",         MSG_SIZE, 2},
    {"Msg",      "/msg",    "PRIVMSG %s :%s\r\n",     MSG_SIZE, 2},
    {"Notice",   "/notice", "NOTICE %s :%s\r\n",      MSG_SIZE, 2},
    {"Whois",    "/whois",  "WHOIS %s\r\n",           MSG_SIZE, 1},
    {"Who",      "/who",    "WHO %s\r\n",             MSG_SIZE, 1},
    {"Motd",     "/motd",   "MOTD\r\n",               MSG_SIZE, 0},
};

char *get_input(void){
    char *input = malloc(MSG_SIZE);
    while(true){
        
        if(fgets(input, MSG_SIZE, stdin) == NULL){
        printf("Fehler beim Lesen.\n");
        continue;
        }


        if(strchr(input, '\n') == NULL){
            int c;
            printf("Eingabe zu lang.\n");
            
            while((c = getchar()) != '\n' && c != EOF);

            continue;
        }

        input[strcspn(input, "\n")] = '\0';

        if(strlen(input) >= 1){
            break;
        }
        else{
            printf("Keine Eingabe.\n");
            continue;
        }
    }

    return input;
}

/*
TODO:   - Sicherheitschecks und Errohandling überprüfen und einbauen
        - Exceptions für einzelne Funktionen etc. ebenfalls
        - In Struct includen ob Channelname oder Nickname mit übergeben werden soll
          und entsprechend implementieren
        - Extrafunktionen für USER NICK und JOIN ? besser integriert als introduce
          und channel
        - Rückgabe-Array wo mit Nickname und Channelname falls diese aktualisiert werden

*/

char *send_command(int sockfd, char *input, char **nickname, char **channelname){
    int count = 0;
    
    if(strncmp(input, "/", 1) != 0){    // Für normale MSG damit man da Command und Channel nicht vorschreiben muss
        if(channelname == NULL){
            printf("Keinem Channel beigetreten\n");   // Dies wird fehlschlagen wenn man von einem Channel parted weil es nicht aktualisiert wird
            free(input);
            return NULL;
        }
        char msg[MSG_SIZE];
        char *comm = "/msg";
        int i = what_command(comm);
        count = snprintf(msg, commands[i].size, commands[i].format, channelname, input);  // Nachricht in das Serverformat bringen
        if(count >= commands[i].size){
            printf("Eingabe zu lang\n");
            free(input);
            return NULL;
        }
        send_msg(sockfd, msg);
        free(input);
        return NULL;    // Hier noch einfügen das die Message geprinted wird
    }
    else{
        char *comm = strtok(input, " ");    // Command aus dem Input nehmen
        if(comm == NULL){
            printf("Eingabe kein Command\n");
            free(input);
            return NULL;
        }

        int i = what_command(comm);
        if(i == -1){
            printf("Unbekannter Command\n");
            free(input);
            return NULL;
        }

        char msg[commands[i].size];

        switch (commands[i].argc){
            case 0: {   // Keine Argumente übergeben
                strcpy(msg, commands[i].format);    // Nachricht in das Serverformat bringen
                send_msg(sockfd, msg);
                free(input);
                return NULL;
                break;
            }
            case 1: {   // Ein Argument übergeben
                char *arg1 = strtok(NULL, " ");
                if(arg1 == NULL){
                    printf("Zu wenige Argumente übergeben\n");
                    free(input);
                    return NULL;
                }
                count = snprintf(msg, commands[i].size, commands[i].format, arg1);  // Nachricht in das Serverformat bringen
                if(count >= commands[i].size){
                    printf("Eingabe zu lang\n");
                    free(input);
                    return NULL;
                }
                send_msg(sockfd, msg);
                free(input);
                return NULL;
                break;
            }
            case 2: {   // Zwei Argumente übergeben
                char *arg1 = strtok(NULL, " ");
                char *arg2 = strtok(NULL, " ");
                if(arg1 == NULL || arg2 == NULL){
                    printf("Zu wenige Argumente übergeben\n");
                    free(input);
                    return NULL;
                }
                count = snprintf(msg, commands[i].size, commands[i].format, arg1, arg2);    // Nachricht in das Serverformat bringen
                if(count >= commands[i].size){
                    printf("Eingabe zu lang\n");
                    free(input);
                    return NULL;
                }
                send_msg(sockfd, msg);
                free(input);
                return NULL;
                break;
            }
            case 3: {   // Drei Argumente übergeben
                char *arg1 = strtok(NULL, " ");
                char *arg2 = strtok(NULL, " ");
                char *arg3 = strtok(NULL, " ");
                if(arg1 == NULL || arg2 == NULL || arg3 == NULL){
                    printf("Zu wenige Argumente übergeben\n");
                    free(input);
                    return NULL;
                }
                count = snprintf(msg, commands[i].size, commands[i].format, arg1, arg2, arg3);  // Nachricht in das Serverformat bringen
                if(count >= commands[i].size){
                    printf("Eingabe zu lang");
                    free(input);
                    return NULL;
                }
                send_msg(sockfd, msg);
                free(input);
                return NULL;
                break; 
            }
            default:{   // Idk wie bzw. wann dieser Fall eintreten sollte
                printf("Error sending message arguments\n");
                free(input);
                return NULL;
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
