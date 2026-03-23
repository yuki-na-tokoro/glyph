#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ncurses.h>
#include <stdarg.h>

#include "client.h"
#include "../protocol.h"
#include "commands.h"


WINDOW *chatfeld;
WINDOW *eingabefeld;
int rows, cols;
int akt_line = 0;   //aktuelle line

void setupscreen(){
    initscr();  
    cbreak();   // eingaben werden sofort verarbeitet 
    noecho();   // eingaben sind nicht aufm screen sichtbar
    keypad(stdscr, TRUE);   // sondertasten funktionieren

    getmaxyx(stdscr, rows, cols);   //setzt rows und cols auf die mögliche größe (terminal)

    chatfeld = newpad(1000, cols);  // neues pad wird erstellt 
    scrollok(chatfeld, TRUE);   //pad lässt sich scrollen(nachrichten immer in neue zeilen geprintet und oben "rutsche" die alten nachrichten aus dem sichtfeld)

    eingabefeld = newwin(1, cols, rows - 1, 0); //eingabefenster wird erstellt
    keypad(eingabefeld, TRUE);

}

void chat_print(const char *text, ...){
    va_list args;
    va_start(args, text);
    vw_printw(chatfeld, text, args); //text wird ins chatfeld geprintet - jetzt auch mit formatierung
    waddch(chatfeld, '\n');

    va_end(args);
    
    if (akt_line < 999){
        akt_line += 1;  //aktuelle line wird erhöht solange kleiner 9999 da padgröße 1000, wenn die größe des pads ereicht ist wird einfach die älteste zeile gelöscht und alles rutscht ein hoch

    }
    int cam_wo = (akt_line > (rows - 2)) ? akt_line - (rows - 2) : 0;   //cam_wo berechnet was im sichtfeld angezeigt wird bzw. wo die unterste zeile des anzuzeigenden ist
    prefresh(chatfeld, cam_wo, 0,0,0,rows-2, cols-1);   // refreshed das "sichtfeld" mit position von cam_wo
}

void get_eingabe(char *buf){
    wmove(eingabefeld, 0, 0);   //moved curser auf 0 0 im eingabefeld
    wclrtoeol(eingabefeld); //cleared das eingabe feld
    mvwprintw(eingabefeld, 0, 0, ">> ");    //setz curser auf 0 0 und printet >>
    wrefresh(eingabefeld);  //refreshed das eingabe feld
    echo(); //nutzereingaben werden sichtbar
    wgetnstr(eingabefeld, buf, 255);    //liest nutzereingabe
    noecho();   //nutzereingabe wird unsichtbar gemacht
    wclrtoeol(eingabefeld);
}
