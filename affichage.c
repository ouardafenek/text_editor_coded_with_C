#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ncurses.h>
#include <unistd.h>

int premier_line = 0;
int current_line = 0;
int current_col = 0;

int getPos(char xOry){
	int x ;
	int y ;
	getyx(stdscr,y,x) ;
	if (xOry=='x') return x ;
	if (xOry=='y') return y ;
	fprintf(stderr, "affichage, getPos, paramètres irroné\n");
	return -1 ;
}



void initialisation_colors(){
    start_color();
	init_pair(1, COLOR_BLACK, COLOR_WHITE);
	init_pair(2, COLOR_RED, COLOR_BLACK);
}


/**
	Affiche le texte selected_text colorier ( soit pour la selection si selection = 1
		ou pour find si selection = 2)

	return 0 si tous c'est bien passé
*/
int color_select_zone(int posy_start, int posx_start, char * selected_text, int selection){
	move(posy_start-premier_line,posx_start); // on va a la positon de depard
    attron(COLOR_PAIR(selection)); // on active la bonne couleur, pour SELECT ou FIND
	addnstr(selected_text,-1); // On print le texte colorer
	//addch('\n');
	attroff(COLOR_PAIR(selection)); // on desactive la couleur
	refresh(); // on affiche sur la cosole le resultat
    return 0 ;
}

void clear_window(){
	clear();
}

int initialisation_fenetre(){
	//lancer la fenetre
 	initscr();
	noecho();
	initialisation_colors();
	return 1;
}


int get_premier_line(){return premier_line;}
int get_LINES(){return LINES;}


int taille_fenetre (int * longueur , int * largeur){
 	*longueur = LINES ;
 	*largeur = COLS ;
 	return premier_line;
}

int quitter_affichage (){
	endwin() ;
	return 0 ;
}


int print_position_curseur(int line, int col){
	move(LINES-1,0);
	clrtoeol();
	printw("(%d,%d)",line,col);
	refresh();
	return 0;
}

/**
	deplace le curseur dans l'affichage, et verifie si il
	faut reafficher le buffer dans l'affichage car on a
	changer de page


	return 	1 si il faut reafficher le buffer car on a KEY_DOWN
			2 si il faut reafficher car on a KEY_UP
			0 sinon
**/
int deplacer (int posy_cur, int posx_cur){
 	current_line = posy_cur;
	current_col = posx_cur;
	if ((current_line-premier_line)>=LINES-1){
		move(0,0);
		refresh();
		premier_line++;
		return 1;
	}
	else if (current_line<premier_line) {
		premier_line--;
		refresh();
		return 2;
	}
	else{
		move (current_line-premier_line , current_col);
		refresh();
		return 0;
	}
}

/**
	Fonction qui permet de positionner curseur a y,x

	!!! Attention, a ne pas utiliser, est utiliser par affiche_buffer dans Editeur
		pour remettre le curseur a 0,0 avant de print les lignes du buffer
*/
void move_print_line(int y, int x){
	move(y,x);
	refresh();
}

/**
	Fonction qui affiche la ligne passer en paramètres, avec un \n
	et qui formate en fonction de si le line_wrapping est activé ou non
*/
int print_line_buffer(char * ligne, int line_wrapping){
	if(line_wrapping==0){
		addnstr(ligne,COLS);
	}
	else{
		addnstr(ligne,-1);
	}
	addch('\n');
	refresh();
	return 0;
}


int print_info(char * info, int line_wrapping, int y, int x){
	print_position_curseur(y,x);
	print_line_buffer(info,line_wrapping);
	return 0;
}


int affichage (char * text, int posx_cur , int posy_cur, int posx_cur_select_start ,
			int posy_cur_select_start , char * selected_text){
	clear() ;  //effacer d'abord la console
 	printw ("%s\n",text);
	refresh();
	//color_select_zone(posy_cur_select_start, posx_cur_select_start, selected_text);
	move (posy_cur , posx_cur) ;
	return 0 ;
}

/**
 * Création de menu pour les raccourcis
 */

int create_menu(){
	WINDOW *w;
    char list[13][7] = { "Cut", "Copy", "Paste", "Save", "Delete", "Quit", "Undo", "Redo",
    "Find", "Open", "Format","Modify","Wrapp"};
    char item[7];
    int ch, i = 0, width = 7;

    w = newwin( 15, 25, (LINES-15) / 2,(COLS-25)/2); // create a new window
    box( w, 0, 0 ); // sets default borders for the window

// now print all the menu items and highlight the first one
    for( i=0; i<13; i++ ) {
        if( i == 0 )
            wattron( w, A_STANDOUT ); // highlights the first item.
        else
            wattroff( w, A_STANDOUT );
        sprintf(item, "%-7s",  list[i]);
        mvwprintw( w, i+1, 2, "%s", item );
    }

    wrefresh( w ); // update the terminal screen

    i = 0;
    noecho(); // disable echoing of characters on the screen
    keypad( w, TRUE ); // enable keyboard input for the window.
    curs_set( 0 ); // hide the default screen cursor.

       // get the input
    while(( ch = wgetch(w)) != '\n'){

                // right pad with spaces to make the items appear with even width.
            sprintf(item, "%-7s",  list[i]);
            mvwprintw( w, i+1, 2, "%s", item );
              // use a variable to increment or decrement the value based on the input.
            switch( ch ) {
                case KEY_UP:
                            i--;
                            i = ( i<0 ) ? 12 : i;
                            break;
                case KEY_DOWN:
                            i++;
                            i = ( i>12 ) ? 0 : i;
                            break;
            }
            // now highlight the next item in the list.
            wattron( w, A_STANDOUT );

            sprintf(item, "%-7s",  list[i]);
            mvwprintw( w, i+1, 2, "%s", item);
            wattroff( w, A_STANDOUT );
    }

 	echo();
 	curs_set( 1 ); // hide the default screen cursor.
    delwin( w );
    return i;
}
