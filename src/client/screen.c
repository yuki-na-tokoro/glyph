#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ncurses.h>
#include <stdarg.h>
#include <time.h>

#include "client.h"
#include "../protocol.h"
#include "commands.h"


WINDOW *chatfeld;
WINDOW *eingabefeld;
WINDOW *infos;
int rows, cols;
//int akt_line = 0;   //aktuelle line

void setupscreen(){
    initscr();  
    cbreak();   // eingaben werden sofort verarbeitet 
    noecho();   // eingaben sind nicht aufm screen sichtbar
    keypad(stdscr, TRUE);   // sondertasten funktionieren
    start_color();
    
    init_pair(1, 118, 17);
    init_pair(2, 17, 118);

    getmaxyx(stdscr, rows, cols);   //setzt rows und cols auf die mögliche größe (terminal)

    chatfeld = newpad(1000, cols);  // neues pad wird erstellt 
    scrollok(chatfeld, TRUE);   //pad lässt sich scrollen(nachrichten immer in neue zeilen geprintet und oben "rutsche" die alten nachrichten aus dem sichtfeld)
    wbkgd(chatfeld,COLOR_PAIR(1));
    prefresh(chatfeld, 0, 0, 0, 0, rows - 5, cols - 1);

    eingabefeld = newwin(3, cols, rows - 3, 0); //eingabefenster wird erstellt
    wbkgd(eingabefeld,COLOR_PAIR(1));
    keypad(eingabefeld, TRUE);
    box(eingabefeld, 0, 0);
    wrefresh(eingabefeld);

    infos = newwin(1, cols, rows - 4, 0);
    wbkgd(infos, COLOR_PAIR(2));
    wattron(infos, A_BOLD);
    wprintw(infos,"[ZEIT]   [NICKNAME]   [SERVERNAME]");
    wrefresh(infos);

}

void chat_print(const char *text, ...){
    //zeit berechnung (fast)wie in logging
    time_t rawtime;
    struct tm *timeinfo;
    char timestr[16];

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(timestr, sizeof(timestr), "%H:%M", timeinfo);
    


    va_list args;
    va_start(args, text);
    wprintw(chatfeld,"[%s]", timestr);
    vw_printw(chatfeld, text, args); //text wird ins chatfeld geprintet - jetzt auch mit formatierung
    waddch(chatfeld, '\n');

    va_end(args);
    
    int y,x;
    getyx(chatfeld, y, x);    //sucht aktuell position im chatfeld
    int cam_wo = (y >= rows - 4)?(y - (rows -4)) : 0;
    /*if (akt_line < 999){
        akt_line += 1;  //aktuelle line wird erhöht solange kleiner 9999 da padgröße 1000, wenn die größe des pads ereicht ist wird einfach die älteste zeile gelöscht und alles rutscht ein hoch

    }
    int cam_wo = (akt_line > (rows - 4)) ? akt_line - (rows - 4) : 0;   //cam_wo berechnet was im sichtfeld angezeigt wird bzw. wo die unterste zeile des anzuzeigenden ist
    */
    prefresh(chatfeld, cam_wo, 0,0,0,rows-5, cols-1);   // refreshed das "sichtfeld" mit position von cam_wo
    doupdate();
}

void timeless_chat_print(const char *text, ...){ //chatprint ohne timestamp
    va_list args;
    va_start(args, text);
    
    vw_printw(chatfeld, text, args); 
    waddch(chatfeld, '\n');

    va_end(args);
    
    int y,x;
    getyx(chatfeld, y, x);    //sucht aktuell position im chatfeld
    int cam_wo = (y >= rows - 4)?(y - (rows -4)) : 0;
    prefresh(chatfeld, cam_wo, 0,0,0,rows-5, cols-1);   // refreshed das "sichtfeld" mit position von cam_wo
    doupdate();
}

