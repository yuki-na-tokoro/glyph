#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "client.h"
#include "../protocol.h"
#include "commands.h"

struct command commands[CLIENT_COMMANDS] = {
    {"Nickname", "/nick",   "NICK %s\r\n",           11,       1},
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

void send_command(int sockfd, char *input){
char *comm = strtok(input, "");
char *args = strtok(NULL, "");

int i = what_command(comm);
if(i == -1){
    printf("Unbekannter Command");
    return;
}

char msg[commands[i].size];

snprintf(msg, commands[i].size, commands[i].format, args);

send_msg(sockfd, msg);

}


int what_command(char *input){
for(int i = 0; i < CLIENT_COMMANDS; i++){
    if(strcmp(commands[i].comm, input) == 0){
        return i;
    }
}

return -1;
}


/*
Verbindung

┌─────────┬─────────────────────────────────┬────────────────────────┐
│ Command │             Syntax              │      Beschreibung      │
├─────────┼─────────────────────────────────┼────────────────────────┤
│ NICK    │ NICK <nickname>\r\n             │ Nickname setzen/ändern │
├─────────┼─────────────────────────────────┼────────────────────────┤
│ USER    │ USER <user> 0 * :<realname>\r\n │ User registrieren      │
├─────────┼─────────────────────────────────┼────────────────────────┤
│ QUIT    │ QUIT [:<reason>]\r\n            │ Verbindung trennen     │
├─────────┼─────────────────────────────────┼────────────────────────┤
│ PASS    │ PASS <password>\r\n             │ Server-Passwort        │
└─────────┴─────────────────────────────────┴────────────────────────┘

Channel

┌─────────┬────────────────────────────────────────┬───────────────────────────┐
│ Command │                 Syntax                 │       Beschreibung        │
├─────────┼────────────────────────────────────────┼───────────────────────────┤
│ JOIN    │ JOIN <#channel>\r\n                    │ Channel betreten          │
├─────────┼────────────────────────────────────────┼───────────────────────────┤
│ PART    │ PART <#channel> [:<reason>]\r\n        │ Channel verlassen         │
├─────────┼────────────────────────────────────────┼───────────────────────────┤
│ TOPIC   │ TOPIC <#channel> [:<topic>]\r\n        │ Topic lesen/setzen        │
├─────────┼────────────────────────────────────────┼───────────────────────────┤
│ NAMES   │ NAMES <#channel>\r\n                   │ User im Channel auflisten │
├─────────┼────────────────────────────────────────┼───────────────────────────┤
│ LIST    │ LIST [<#channel>]\r\n                  │ Channels auflisten        │
├─────────┼────────────────────────────────────────┼───────────────────────────┤
│ KICK    │ KICK <#channel> <user> [:<reason>]\r\n │ User kicken               │
├─────────┼────────────────────────────────────────┼───────────────────────────┤
│ INVITE  │ INVITE <user> <#channel>\r\n           │ User einladen             │
├─────────┼────────────────────────────────────────┼───────────────────────────┤
│ MODE    │ MODE <#channel> <flags>\r\n            │ Channel-Modus setzen      │
└─────────┴────────────────────────────────────────┴───────────────────────────┘

Nachrichten

┌─────────┬─────────────────────────────────┬────────────────────────────────┐
│ Command │             Syntax              │          Beschreibung          │
├─────────┼─────────────────────────────────┼────────────────────────────────┤
│ PRIVMSG │ PRIVMSG <target> :<message>\r\n │ Nachricht an Channel oder User │
├─────────┼─────────────────────────────────┼────────────────────────────────┤
│ NOTICE  │ NOTICE <target> :<message>\r\n  │ Wie PRIVMSG, kein Auto-Reply   │
└─────────┴─────────────────────────────────┴────────────────────────────────┘

Informationen

┌─────────┬────────────────────┬───────────────────────┐
│ Command │       Syntax       │     Beschreibung      │
├─────────┼────────────────────┼───────────────────────┤
│ WHOIS   │ WHOIS <user>\r\n   │ Info über einen User  │
├─────────┼────────────────────┼───────────────────────┤
│ WHO     │ WHO <#channel>\r\n │ User in einem Channel │
├─────────┼────────────────────┼───────────────────────┤
│ MOTD    │ MOTD\r\n           │ Message of the Day    │
├─────────┼────────────────────┼───────────────────────┤
│ PING    │ PING <token>\r\n   │ Verbindung prüfen     │
├─────────┼────────────────────┼───────────────────────┤
│ PONG    │ PONG <token>\r\n   │ Antwort auf PING      │
└─────────┴────────────────────┴───────────────────────┘

*/