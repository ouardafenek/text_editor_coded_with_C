#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ncurses.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "chargement.h"
#include "buffer.h"

int premier_line = 0;
int current_line = 0;
int current_col = 0;
Buffer * buffer;



int print_buffer(){
    clear();
    int i = premier_line;

    char * line;
    move(0,0);
    for (int i  = premier_line ;i<premier_line+LINES;i++){
        line = getLineBuffer(buffer,i);
        printw("%s\n",line);
    }
    move(LINES-2,0);
    refresh();
    return i;
}


int main(int argc, char const *argv[]) {
    char * buf_fichier;
    char * nomDeFichier= "./test_scrolling.c";
    premier_line = 0;
    current_line = 0;
    current_col = 0;

    if(chargement(&buf_fichier,nomDeFichier)==(-1)){
        printf("Impossible de charger le fichier\n");
        free(buf_fichier);
        exit(0);
    }

    buffer = newBuffer(10);
    addTextBuffer(buffer,buf_fichier);
    //free(buf_fichier);

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    scrollok(stdscr,TRUE);
    clear();
    print_buffer();
    move(0,0);

    refresh();

    int ch;
    while ((ch = getch()) != KEY_F(5)) {
        switch (ch) {
            case KEY_DOWN:
                current_line++;
                if ((current_line-premier_line+1)>LINES){
                    premier_line++;
                    print_buffer();
                }
                else{
                    move(current_line-premier_line-1,current_col);
                }
                break;
            case KEY_UP:
                if (current_line<=premier_line) {
                    current_line--;
                    premier_line--;
                    print_buffer();
                    move(0,0);
                }
                else{
                    current_line--;
                    move(current_line-premier_line-1,current_col);
                }
                break;
        }
        /*
        move(0,current_col);
        printw("%d  %d   %d|",current_line,premier_line,LINES);
        move(current_line-premier_line-1,current_col);*/
        refresh();
    }
    endwin();
    return 0;
}
