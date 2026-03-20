#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ncurses.h>

#include "client.h"
#include "../protocol.h"
#include "commands.h"


WINDOW *chatfeld;
WINDOW *eingabefeld;
int rows, cols;
int akt_line = 0;

void setupscreen(){
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    getmaxyx(stdscr, rows, cols);

    chatfeld = newpad(1000, cols);
    scrollok(chatfeld, TRUE);

    eingabefeld = newwin(1, cols, rows - 1, 0);
    keypad(eingabefeld, TRUE);

}

void chat_print(const char *text){
    wprintw(chatfeld,"%s\n", text);
    if (akt_line < 999){
        akt_line += 1;

    }
    int cam_wo = (akt_line > (rows - 2)) ? akt_line - (rows - 2) : 0;
    prefresh(chatfeld, cam_wo, 0,0,0,rows-2, cols-1);
}

void get_eingabe(char *buf){
    wmove(eingabefeld, 0, 0);
    wclrtoeol(eingabefeld);
    mvwprintw(eingabefeld, 0, 0, ">> ");
    wrefresh(eingabefeld);
    echo();
    wgetnstr(eingabefeld, buf, 255);
    noecho();
    wclrtoeol(eingabefeld);
}
