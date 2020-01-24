#ifndef AFFICHAGE_H
#define AFFICHAGE_H

/**
	Retourner la position courante x si xOry=='x'
	Retourner la position courante y si xOry=='y'
	retourne -1 sinon
**/
int getPos(char xOry) ;


/**
	Préparer la fenetre à l'utilisation des couleurs

**/
void initialisation_colors();

/**
	Colorer une zone de selection
	@param: posx_start : la position x de début de la selection
			posy_start : la postion y du début de la selection
			selected_text : le texte selectionné
	@retour: 0
**/

int color_select_zone(int posx_start, int posy_start, char * selected_text,int selection);



/**
    Se contente d'effacer la fenetre

**/
void clear_window();


/**
	Getter pour premier_line
*/
int get_premier_line();

/**
	Getter pour LINES
*/
int get_LINES();

/**
	renvoie dans *longueur la longuer de la fenetre et dans *largeur sa largeur
	retour : premier_line

**/
int taille_fenetre (int * longueur , int * largeur) ;


/**
	Fonction qui permet de positionner curseur a y,x

	!!! Attention, a ne pas utiliser, est utiliser par affiche_buffer dans Editeur
		pour remettre le curseur a 0,0 avant de print les lignes du buffer
*/
void move_print_line(int, int);

int print_position_curseur(int line, int col);

/**
    affiche la ligne dans la console, effectue un retour a la ligne suivant avec \n
    et formate en fonction de si line_wrapping est activé ou non
*/
int print_line_buffer(char * ligne, int line_wrapping);

int print_info(char * info, int line_wrapping, int y, int x);

/**
	afficher la chaine de caractères text
	afficher également le cureur à la position (posx_cur , posy_cur)
	appliquer des couleurs differentes à la zone de selection designée pas
		le début de la selection (deux cordonnées x et y) et par le texte selectionné
	retour :
		elle renvoie 0

*/
int affichage (char * text, int posx_cur , int posy_cur, int posx_cur_select_start , int posy_cur_select_start , char * selected_text) ;

/**
 *
 * Affiche uniquement la ligne posx_cur au lieux de tous re-afficher
 *
 */
int affichage_ligne (char * text, int posx_cur , int posy_cur, int posx_cur_select_start , int posy_cur_select_start , char * selected_text) ;



/**
*
*   Initiale la fenetre
*
*
*   retourne -1 si Erreur sinon un entier positif ou nul
*
*/
int initialisation_fenetre();

/**
*
*	Fermer la fenetre de l'éditeur , retourne 0
*
*/
int quitter_affichage () ;

/**
*	déplacer le curseur vers les positions indiquéees
*
*/
int deplacer(int posy_cur , int posx_cur) ;
/**
*  Création du menu des raccourcis
*/
int create_menu();
#endif
