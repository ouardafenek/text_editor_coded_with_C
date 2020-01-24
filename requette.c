#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ncurses.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <errno.h>

#define ESC 27
#define MAX_LIG 255
#define NB_RACC 14

#define KEY_CTRL(x)    ((x) & 0x1f)

//les retour de la fonction getentry_keyboard
//Type des commande
#define INSERT 0
#define MOVE 1
#define SELECT 2
#define COPY 3
#define CUT 4
#define PASTE 5
#define DELETE 6
#define SAVE 7
#define QUIT 8
#define DELETEBACK 9 //delete back space
#define REDIMENTION 10
#define UNDO 11
#define REDO 12
#define FIND 13
#define OPEN 14
#define FORMAT 15
#define MODIFY 16
#define MENU 17
#define LINE_WRAPPING 18

//les retours de la fonction getentry_mouse
#define LEFT_B_PRESSED 1 //left button pressed
#define LEFT_B_RELEASED 0  //left button released



typedef struct command {
	MEVENT event ; //remplie lorsque une entrée est saisie avec la souris
	char c ;  //caractère à insérer ou avec quoi remplacer la selection
				// dans les cas ou la commande est la selection,  ou bien MOVE, ce caracère indiquera la direction
				// ie il vaut :
				// u : Up , d : Down , r : Right , l : Left
}command ;




//Raccourcis clavier qu'on lira du fichier de configurations

//raccourcis[0] = Cut
//raccourcis[1] = Copy
//raccourcis[2] = Paste
//raccourcis[3] = Save
//raccourcis[4] = Delete
//raccourcis[5] = Quit
//raccourcis[6] = UNDO
//raccourcis[7] = REDO
//raccourcis[8] = FIND
//raccourcis[9] = OPEN
//raccourcis[10]= FORMAT
//raccourcis[11]=MODIFY
//raccourcis[12]=MENU
char * raccourcis [NB_RACC] ; //pour le moment on a 6 raccourcis


char * second_word (char * s){
	//retourne le deuxième mot de la chaine s
	//le séparateur étant le caractère blanc et le retour à la ligne
	char * tok = strtok (s , " ") ;
	tok = strtok (NULL , " \n") ;
	return tok ;
}

int read_configuration_file (){
	//lire le fichier de configuration
	// ce derner porte le nom de configuration.config
	//il faut suivre la syntaxe imposé
	//devant chaque nom de commande écrire le raccourci correspondant
	FILE * f  = fopen ("configuration.config" , "r") ;
	if (f==NULL){
		fprintf(stderr, "configuration.config : %s \n",strerror(errno) );
		return -1 ;
	}
	char * s = malloc (sizeof (char) * MAX_LIG) ;
	int i = 0 ;
	while (fgets(s , MAX_LIG*sizeof(char) , f) != NULL && i<NB_RACC){
		raccourcis[i] = malloc (sizeof (char) * 10 ) ;
		strcpy ( raccourcis[i], second_word(s)) ;
		i++ ;
	}
	fclose (f) ;
	return 0 ;
}

//retourne le type de la commande
// -1 en cas d'erreurs (l'entrée ne correspond pas à une entrée clavier)
//remplie la structutre commande
int  getentry(command * commande){
	//récupérer l'entrée
 	commande->c = '\0' ;
	int c  = wgetch(stdscr) ;
		//cas de la selection ...
		if (c == (int)KEY_SRIGHT){ //shift right
			commande->c = 'r' ;
			return SELECT ;
		}
		else if (c== (int)KEY_SLEFT){ //shift left
			commande->c = 'l' ;
			return SELECT ;
		}

		// Dans le cas d'un redimensionnement de la fenetre
		else if (c==410){
			return REDIMENTION ;
		}

		//shift up, la touche n'est pas défini en ncurses, il se trouve que ça correspond à

		//cas tabulation
		else if (c==KEY_STAB){
			commande->c ='\t' ;
			return INSERT ;

		}
		else if (c==KEY_BACKSPACE){
			return  DELETEBACK;
		}
		//cas des autres raccourcis
		else if (strcmp(keyname(c),raccourcis[0])==0){
 			return CUT;
 		}
 		else if (strcmp(keyname(c),raccourcis[1])==0){
 			return  COPY ;
 		}
 		else if (strcmp(keyname(c),raccourcis[2])==0){
 			return PASTE ;
 		}
 		else if (strcmp(keyname(c),raccourcis[3])==0){
 			return SAVE ;
 		}
 		else if (strcmp(keyname(c),raccourcis[4])==0){
 			return DELETE ;
 		}
 		else if (strcmp(keyname(c),raccourcis[5])==0){
			return QUIT ;
 		}
 		else if (strcmp(keyname(c),raccourcis[6])==0){
			return UNDO ;
 		}
		else if (strcmp(keyname(c),raccourcis[7])==0){
			return REDO ;
 		}
 		else if (strcmp(keyname(c),raccourcis[8])==0){
			return FIND ;
 		}
 		else if (strcmp(keyname(c),raccourcis[9])==0){
			return OPEN ;
 		}
 		else if (strcmp(keyname(c),raccourcis[10])==0){
			return FORMAT ;
 		}
 		else if (strcmp(keyname(c),raccourcis[11])==0){
			return MODIFY ;
 		}
 		else if (strcmp(keyname(c),raccourcis[12])==0){
			return MENU ;
 		}
		else if (strcmp(keyname(c),raccourcis[13])==0){
			return LINE_WRAPPING ;
		}
		else if (c == (int)ESC){ // echap
			return QUIT ;
		}

 		//on va maintenant gérer le cas des flèches,
 		// le type de la commande sera donc MOVE
 		// le caractère c determinera la direction du MOVE
 		else if(c== (int)KEY_RIGHT){
			commande->c = 'r' ;
 			return MOVE ;
 		}
 		else if(c== (int)KEY_LEFT){
 			commande->c = 'l' ;
 			return MOVE ;
 		}
 		else if(c== (int)KEY_UP){
			commande->c = 'u' ;
 			return MOVE ;
 		}
 		else if(c== (int)KEY_DOWN){
			commande->c = 'd' ;
 			return MOVE ;
 		}
 		else if (c==KEY_MOUSE){
			// Mouse event.
			if (OK == getmouse(&(commande->event))){
				if ((commande->event).bstate & BUTTON1_PRESSED){
						return  LEFT_B_PRESSED;
				}
				else if ((commande->event).bstate & BUTTON1_RELEASED){
						return LEFT_B_RELEASED;
				}
			}
			else{
						 //mouse move
			}
		}
 		//insertion d'un caractère
 		else{
 			commande->c = c ;
 			return INSERT ;
 		}

 		//afficher_commande(commande) ;
		return -1 ;
}

int initialisation_requete (){
	read_configuration_file() ;
	raw () ; //desactiver les raccourcis standard et les signaux d'interruption
	keypad(stdscr,TRUE); //sets "using-keypad" to true in order to reconize arrows
	mousemask(ALL_MOUSE_EVENTS, NULL);
	/*if (has_mouse()==FALSE){
 		return -1  ;
 	}*/
	return 0 ;
}

char * demande_nom_fichier (){
	char * nom_fichier = malloc (sizeof(char)*50);
	echo();
	if (scanw("%s",nom_fichier)==ERR){
		fprintf(stderr, "requette, demande_nom_fichier, erreur\n");
		return NULL ;
	}
	noecho();
	return nom_fichier;
}
