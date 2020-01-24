#ifndef REQUETTE_H
#define REQUETTE_H
#include <ncurses.h>


//les retours de la fonction getentry_keyboard
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



struct command {
	MEVENT event ; //remplie lorsque une entrée est saisie avec la souris
									//elle permet de savoir a quel endroit l'événement a eu lieu
	char c ;  //caractère à insérer ou avec quoi remplacer la selection
				// dans les cas ou la commande est la selection,  ou bien MOVE, ce caracère indiquera la direction
				// ie il vaut :
				// u : Up , d : Down , r : Right , l : Left
};

/**
retourne le deuxième mot de la chaine s
le séparateur étant le caractère blanc et le retour à la ligne
**/
char * second_word (char * s);


char * demande_nom_fichier();

/**
lire le fichier de configuration
ce derner porte le nom de configuration.txt
il faut suivre la syntaxe imposé
devant chaque nom de commande écrire le raccourci correspondant
**/
int read_configuration_file ();


/**
	une fonction qui attend un événement soit :
		de la souris
			elle remplie la structure MEVENT dans la commande pour savoir à quel endroit le clic a eu lieu et de quel type est il
			Mouse at row event.y , column   event.x
			retour : LEFT_B_PRESSED ou LEFT_B_RELEASED dans ces cas
		du clavier :

			elle retourne le type de l'evenement select, copy, insert, ...
			et remplie le champs c avec des caractères r,l,u,d pour indiquer la direction de la commande
			(left, right, up and down)
		retourne -1 en cas d'erreur
*/
int getentry(struct command * commande) ;

/**
	commande d'initialisation et de préparation pour la lecture des requetes
	elle lit le fichier de configuration
	desactive les signaux d'interruption et les raccourcis clavier
	autorise les entrées de la souris
	retour 0 : si tout se passe bien
		  -1 : si la souris n'est pas detectée
**/
int initialisation_requete () ;

#endif
