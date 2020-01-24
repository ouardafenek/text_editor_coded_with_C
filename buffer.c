#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "buffer.h"
#include "position.h"

struct Buffer{
	int cursorLine;
	int cursorCol;
	int selectLine;
	int selectCol;
	int nbLine;
	int nbLineMax;
	char** lines;
	char* clipboard;
	int auto_fill;
	int auto_fill_width;
};


/**
 * Ajoute une ligne dans un buffer
 *
 *buffer: le buffer dans lequel ajouter une ligne
 *
 * Retour: 0, s'il n'y a pas de probleme, -1 sinon
 */
static int bufferSizeUp(struct Buffer* buffer){

	buffer->nbLine += 1;

	// Si en ajoutant une ligne, on depasse la capacite max du buffer
	if(buffer->nbLine > buffer->nbLineMax){
		// Alors, on double la capacite max
		buffer->nbLineMax *= 2;
		char** tmp = realloc(buffer->lines, buffer->nbLineMax*sizeof(char*));
		if(tmp == NULL){
			perror("Erreur allocation: buffer#bufferSizeUp");
			return -1;
		}
		buffer->lines = tmp;
	}

	return 0;

}




/**
 * Change la colonne du curseur de position
 * La nouvelle colonne doit etre entre 0 et la longueur de la ligne
 * actuelle du buffer
 *
 * buffer: le buffer
 *
 * pos: la colonne du curseur de position
 *
 * Retour: 0 si le changement est effectue, -1 sinon
 */
static int setCurColBuffer(struct Buffer* buffer, int pos){
	// Si la colonne est invalide
	if(pos < 0 || pos > strlen(getLineBuffer(buffer, getCurLineBuffer(buffer)))){
		return -1;
	}
	buffer->cursorCol = pos;
	return 0;
}

/**
 * Change la ligne du curseur de position
 * La colonne du curseur n'est pas change a moins
 * que la ligne de destination ne comporte pas assez de caracteres,
 * Dans ce cas, le curseur est place a la fin de la ligne
 *
 * buffer: le buffer
 *
 * pos: la ligne du curseur de position
 *
 * Retour: 0 si le changement est effectue, -1 sinon
 */
static int setCurLineBuffer(struct Buffer* buffer, int pos){
	// Si la nouvelle position est impossible
	if(pos < 0 || pos >= getNbLineBuffer(buffer)){
		return -1;
	}
	buffer->cursorLine = pos;
	// Si la colonne n'est pas valide
	if(strlen(getLineBuffer(buffer, pos)) < getCurColBuffer(buffer)){
		// On le met a la fin de la ligne
		setCurColBuffer(buffer, strlen(getLineBuffer(buffer, pos)));
	}

	return 0;
}




/**
 * Change la ligne du curseur de selection
 * La nouvelle colonne doit etre entre 0 et la longueur de la ligne
 * actuelle du buffer
 *
 * buffer: le buffer
 *
 * pos: la ligne du curseur de selection
 *
 * Retour: 0 si le changement est effectue, -1 sinon
 */
static int setCurSelectColBuffer(struct Buffer* buffer, int pos){
	// Si la colonne est invalide
	if(pos < 0 || pos > strlen(getLineBuffer(buffer, getCurSelectLineBuffer(buffer)))){
		return -1;
	}
	buffer->selectCol = pos;
	return 0;
}
/**
 * Change la ligne du curseur de selection
 * La colonne du curseur n'est pas change a moins
 * que la ligne de destination ne comporte pas assez de caracteres,
 * Dans ce cas, le curseur est place a la fin de la ligne
 *
 * buffer: le buffer
 *
 * pos: la ligne du curseur de selection
 *
 * Retour: 0 si le changement est effectue, -1 sinon
 */
static int setCurSelectLineBuffer(struct Buffer* buffer, int pos){
	// SI la ligne n'existe pas
	if(pos < 0 || pos >= getNbLineBuffer(buffer)){
		return -1;
	}
	buffer->selectLine = pos;
	// Si la colonne n'est plus valide
	if(strlen(getLineBuffer(buffer, pos)) < getCurSelectColBuffer(buffer)){
		setCurSelectColBuffer(buffer, strlen(getLineBuffer(buffer, pos)));
	}
	return 0;
}

/**
 * Le curseur de selection d'un buffer copie la place du curseur de position
 *
 * buffer: le buffer dans lequel effectuer les operations
 */
static void copy_curPos_curSel(struct Buffer* buffer){
	setCurSelectLineBuffer(buffer, getCurLineBuffer(buffer));
	setCurSelectColBuffer(buffer, getCurColBuffer(buffer));
}



int retrouve_premier_buffer(struct Buffer* buffer){
	if(getCurLineBuffer(buffer) == getCurSelectLineBuffer(buffer)){
		return getCurColBuffer(buffer) - getCurSelectColBuffer(buffer);
	}
	return getCurLineBuffer(buffer) - getCurSelectLineBuffer(buffer);
}

/**
 * Compte le nombre de caracters dans la zone de selection d'un buffer (sauts de ligne inclus)
 *
 * buffer: le buffer
 *
 * Retour: le nombre de caractere dans la zone de selection
 */
static int nb_char_selection_buffer(struct Buffer* buffer){
	int nb_char = 0;

	int dx;
	int dy;
	int fx;
	int fy;

	int premier = retrouve_premier_buffer(buffer);

	// Initialisation des variables selon la position des curseurs
	if(premier < 0){
		dx = getCurLineBuffer(buffer);
		dy = getCurColBuffer(buffer);
		fx = getCurSelectLineBuffer(buffer);
		fy = getCurSelectColBuffer(buffer);
	}else{
		if(premier > 0){
			fx = getCurLineBuffer(buffer);
			fy = getCurColBuffer(buffer);
			dx = getCurSelectLineBuffer(buffer);
			dy = getCurSelectColBuffer(buffer);
		}else{
			return 1;
		}
	}

	for(int i = dx ; i <= fx ; ++i){ // Compter le nombre de caractere total dans les lignes
		nb_char += strlen(getLineBuffer(buffer, i));
	}

	nb_char += ((fx-dx)); //Les sauts de lignes

	nb_char -= dy; //On retire les caracters avant la zone sur la 1e ligne

	nb_char -= strlen(getLineBuffer(buffer, fx))-fy; //Idem pour la fin de la zone

	return nb_char;
}


/**
 * Ajoute une nouvelle ligne a la fin d'un buffer
 *
 * buffer: le buffer dans lequel ajouter une ligne
 *
 * Retour l'adresse de la nouvelle ligne, NULL lors de l'echec d'un ajout
 */
static char* newLineBuffer(struct Buffer* buffer){
	if(bufferSizeUp(buffer)){ // Ajoute une ligne
		fprintf(stderr, "Erreur lors de l'ajout d'une ligne dans le buffer");
		return NULL;
	}

	// Allocation de la ligne
	buffer->lines[buffer->nbLine-1] = malloc(1);
	buffer->lines[buffer->nbLine-1][0] = '\0';

	return buffer->lines[buffer->nbLine-1];
}

/**
 * Determine s'il existe une zone de selection dans un buffer
 *
 * buffer, le buffer dans lequel chercher
 *
 * Retour: 1 s'il existe une zone de selection, sinon 0
 */
static int selectZoneExistBuffer(struct Buffer* buffer){
	return getCurLineBuffer(buffer) != getCurSelectLineBuffer(buffer) || getCurColBuffer(buffer) != getCurSelectColBuffer(buffer);
}

/**
 * Retire la zone de selection
 *
 * Retour: les caracteres retirés dans la zone de selection
 */
static char* removeSelectionZoneBuffer(struct Buffer* buffer){

	int nb_char_selection = nb_char_selection_buffer(buffer);

	// Si le curseur de position est en debut de la zone
	if(retrouve_premier_buffer(buffer) < 0){
		// On le met a la fin pour utiliser la fonction suppression de caracteres
		// Cela a pour effet de supprimer la zone de selection
		setCurLineBuffer(buffer, getCurSelectLineBuffer(buffer));
		setCurColBuffer(buffer, getCurSelectColBuffer(buffer));
	}

	copy_curPos_curSel(buffer);
	return removeCharBuffer(buffer, nb_char_selection);

}


char* getSelectionZoneBuffer(struct Buffer* buffer){

	int premier = retrouve_premier_buffer(buffer);

	// S'il n'y a pas de zone de selection
	if(premier == 0){
		return calloc(1, 1);
	}

	int x_premier, y_premier;
	int x_dernier, y_dernier;

	// Initialisation des variables a la bonne valeur
	if(premier < 0){
		x_premier = getCurLineBuffer(buffer);
		x_dernier = getCurSelectLineBuffer(buffer);
		y_premier = getCurColBuffer(buffer);
		y_dernier = getCurSelectColBuffer(buffer);
	}else{
		x_dernier = getCurLineBuffer(buffer);
		x_premier = getCurSelectLineBuffer(buffer);
		y_dernier = getCurColBuffer(buffer);
		y_premier = getCurSelectColBuffer(buffer);
	}


	char* retour = calloc(1, 1);

	// Ajout de toutes les lignes
	for(int i = x_premier ; i <= x_dernier ; ++i){
		char * line = getLineBuffer(buffer, i);
		retour = realloc(retour, strlen(retour) + strlen(line) + 2);

		if(retour == NULL){
			perror("Erreur d'allocation mémoire buffer#getSelectionZone");
			return NULL;
		}

		strcat(retour, line);
		strcat(retour, "\n");

	}

	// Determine la longueur de la chaine que devrait avoir la zone de selection
	int longueur_finale = strlen(retour) - y_premier - (strlen(getLineBuffer(buffer, x_dernier)) - y_dernier) -1;

	// Suppression des ceracteres en trop (debut et fin) et reallocation
	memmove(retour, retour + y_premier, longueur_finale);
	retour[longueur_finale] = '\0';
	retour = realloc(retour, longueur_finale +1);

	return retour;
}


struct Buffer* newBuffer(int ligne){
	struct Buffer* retour = malloc(sizeof(struct Buffer));

	retour->cursorLine = 0;
	retour->cursorCol = 0;
	retour->selectLine = 0;
	retour->selectCol = 0;

	retour->nbLineMax = ligne;
	retour->nbLine = 0;
	retour->lines = malloc(sizeof(char*)*ligne);
	newLineBuffer(retour);
	retour->clipboard = malloc(1);
	retour->clipboard[0] = '\0';

	retour->auto_fill = 0;
	retour->auto_fill_width = 79;

	return retour;
}


int getNbLineBuffer(struct Buffer* buffer){
	return buffer->nbLine;
}


/**
 * Ajoute une ligne a une position indiquee
 * Toutes les lignes sont decalees vers le bas (no ligne + 1)
 * La gestion de la taille de la liste des lignes est automatique
 * Si le no de ligne de la nouvelle ligne est apres la dernière ligne
 * on ajoute une nouvelle ligne a la fin
 *
 * buffer: le buffer dans lequel ajouter une nouvelle ligne
 *
 * noLigne: la ligne de la nouvelle ligne
 *
 * Retour: l'adresse de la nouvelle ligne (char*), NULL en cas d'echec
 */
static char* newLineInsertBuffer(struct Buffer* buffer, int noLine){
	// Si on souhaite ajouter une ligne apres la dernière ligne
	if(noLine > buffer->nbLine){
		return newLineBuffer(buffer); // On ajoute une nouvelle ligne a la fin
	}

	if(noLine < 0){ // le no de la nouvelle ligne est negatif
		fprintf(stderr, "noLine < 0 : buffer.c#newLineInsertBuffer");
		return NULL;
	}

	if(bufferSizeUp(buffer)){ // Ajout d'une ligne
		fprintf(stderr, "Erreur lors de l'ajout d'une ligne dans le buffer");
		return NULL;
	}

	// deplacement des lignes qui suivent
	memmove(buffer->lines+noLine+1, buffer->lines+noLine, (buffer->nbLine - noLine) * sizeof(char*));

	// Allocation de la nouvelle ligne
	buffer->lines[noLine] = malloc(1);
	buffer->lines[noLine][0] = '\0';

	return buffer->lines[noLine];
}


void freeBuffer(struct Buffer* buffer){
	// Liberation des ressources pour chaque ligne
	for(int i = 0 ; i < buffer->nbLine ; ++i){
		free(buffer->lines[i]);
	}
	free(buffer->lines); // liberation du tableau de chaine de caracteres
	free(buffer->clipboard); // Liberation du presse papier
	free(buffer); // Liberation du buffer
}

char* getLineBuffer(struct Buffer* buffer, int noLine){
	// Si le no de la ligne demandee n'existe pas, on retourne NULL
	return (noLine > getNbLineBuffer(buffer) || noLine < 0)?NULL:buffer->lines[noLine];
}

/**
 * Realloue un emplacement pour une ligne d'un buffer
 *
 * buffer: le buffer dans lequel se trouve la ligne
 *
 * noLine: le no de la ligne a reallouer
 *
 * size: la nouvelle taille du buffer
 *
 * Retour: l'adresse de la ligne realloue, NULL si la reallocation a echoue
 */
static char* reallocBuffer(struct Buffer* buffer, int noLine, size_t size){

	char* tmp = realloc(buffer->lines[noLine], size);
	if(tmp == NULL){
		perror("Erreur lors de la reallocation : buffer#reallocBuffer");
		return NULL;
	}

	buffer->lines[noLine] = tmp;

	return buffer->lines[noLine];
}


/**
 * Ajoute du texte a l'emplacement du curseur
 * Le texte ne peut pas contenir de '\n', c'est traite dans une autre fonction
 *
 * buffer: le buffer dans lequel ajouter du texte
 *
 * txt: le texte a ajouter dans un buffer
 */
static int _add_txt_buffer(struct Buffer* buffer, char* txt){

	char* line = getLineBuffer(buffer, getCurLineBuffer(buffer));

	// Reallocation de la memoire pour accueillir le nouveau texte
	if(reallocBuffer(buffer, getCurLineBuffer(buffer), strlen(line) + strlen(txt) + 1) == NULL){
		return -1;
	}

	// On récupère la bonne ligne
	line = getLineBuffer(buffer, getCurLineBuffer(buffer));

	// Decale le texte a droite de la zone d'insertion
	memmove(
		line + getCurColBuffer(buffer) + strlen(txt),
		line + getCurColBuffer(buffer),
		strlen(line + getCurColBuffer(buffer)) + 1
	);

	// Copie du texte a ajouter
	memcpy(line + getCurColBuffer(buffer), txt, strlen(txt));

	// Deplacement des curseurs
	setCurColBuffer(buffer, getCurColBuffer(buffer) + strlen(txt));
	setCurSelectColBuffer(buffer, getCurColBuffer(buffer));
	copy_curPos_curSel(buffer);


	return 0;
}

/**
 * Ajoute une ligne dans le buffer
 * La nouvelle ligne est ajoute apres la position du curseur
 * Les caracteres sui suivent le curseur sont deplace sur la ligne du bas
 *
 * buffer: le buffer dans lequel ajouter une ligne
 */
static void _add_line_buffer(struct Buffer* buffer){
	// Ajout d'une nouvelle ligne sur la ligne suivante
	newLineInsertBuffer(buffer, getCurLineBuffer(buffer) + 1);


	// Reallocation de la ligne suivante avec une taille correspondante au restant de la ligne superieur
	reallocBuffer(buffer,
		getCurLineBuffer(buffer) + 1,
		strlen(getLineBuffer(buffer, getCurLineBuffer(buffer))) -
			getCurColBuffer(buffer) + 1
		);

	// Copie du reste de la ligne sur le ligne suivante
	strcpy(getLineBuffer(buffer, getCurLineBuffer(buffer)+1),
		getLineBuffer(buffer, getCurLineBuffer(buffer)) +
			getCurColBuffer(buffer)
	);


	// Reallocation de la ligne precedente
	reallocBuffer(buffer, getCurLineBuffer(buffer), getCurColBuffer(buffer)+1);
	getLineBuffer(buffer, getCurLineBuffer(buffer))[getCurColBuffer(buffer)] = '\0';

	// Deplacement du curseur
	setCurColBuffer(buffer, 0);
	setCurLineBuffer(buffer, getCurLineBuffer(buffer)+1);
	copy_curPos_curSel(buffer);
}

int addTextBuffer(struct Buffer* buffer, char* txt){
	int pos_txt = 0;
	int pos_line = 0;
	size_t lineSize = 100;
	char* line = malloc(lineSize);

	// On retire la zone de selection si elle existe
	if(selectZoneExistBuffer(buffer)){
		free(removeSelectionZoneBuffer(buffer));
	}

	// Separation de txt sur les lignes
	for(pos_txt = 0 ; pos_txt < strlen(txt) ; ++pos_txt){
		char c = txt[pos_txt];
		if(c == '\n'){ // Si on trouve un caractere de fin de chaine
			line[pos_line] = '\0';
			_add_txt_buffer(buffer, line);
			_add_line_buffer(buffer);
			pos_line = 0;

		}else{ // Sinon, on ajoute le caractere dans la chaine de caracteres a ajouter
			line[pos_line] = c;
			++pos_line;

			if(pos_line >= lineSize){ // Au cas ou la ligne est trop longue
				lineSize *= 2;
				line = realloc(line, lineSize);
			}
		}
	}
	line[pos_line] = '\0';

	_add_txt_buffer(buffer, line);
	free(line);

	// Si l'option auto_fill est activee
	if(txt[0] != '\n' && buffer->auto_fill){
		if(strlen(getLineBuffer(buffer, getCurLineBuffer(buffer))) > buffer->auto_fill_width){
			printf("Passe : ligne %d\n", getCurLineBuffer(buffer));
			formatColumnWidthBuffer(buffer);
		}
	}

	return (int)strlen(txt);
}

char* getTextBuffer(struct Buffer* buffer){
	return getTextPartielBuffer(buffer, 0, getNbLineBuffer(buffer)-1);
}

char* getTextPartielBuffer(struct Buffer* buffer, int debut, int fin){
	char* retour = malloc(1);
	retour[0] = '\0';

	if(debut < 0){
		fprintf(stderr, "ligne debut getText negatif");
		return NULL;
	}

	if(fin > getNbLineBuffer(buffer)){
		fprintf(stderr, "Ligne fin getText trop grand");
		return NULL;
	}

	if(debut > fin){
		fprintf(stderr, "Erreur debut apres la fin");
		return NULL;
	}

	// Ajout des textes
	for(int i = debut ; i <= fin ; ++i){
		if(i != 0){ // Ajout d'un caractere de saut de ligne a partir de la deuxieme ligne
			char* tmp = realloc(retour, strlen(retour)+2);
			if(tmp == NULL){
				perror("Probleme reallocation getTextBuffer : buffer#getTextPartielBuffer");
				return NULL;
			}
			retour = tmp;
			strcat(retour, "\n");
		}

		char* line = getLineBuffer(buffer, i);

		char* tmp = realloc(retour, strlen(retour) + strlen(line) + 1);
		if(tmp == NULL){
			perror("Probleme reallocation getTextBuffer N2 : buffer#getTextPartielBuffer");
			return NULL;
		}
		retour = tmp;

		strcat(retour, line);
	}

	return retour;
}

int getCurLineBuffer(struct Buffer* buffer){
	return buffer->cursorLine;
}

int getCurColBuffer(struct Buffer* buffer){
	return buffer->cursorCol;
}

int getCurSelectLineBuffer(struct Buffer* buffer){
	return buffer->selectLine;
}

int getCurSelectColBuffer(struct Buffer* buffer){
	return buffer->selectCol;
}
char * getClipBoardBuffer(struct Buffer* buffer){
	return buffer->clipboard;
}


int moveCursorBuffer(struct Buffer* buffer, int direction, int distance){
	for(int i = 0 ; i < distance ; ++i){
		switch(direction){
		case BUFFER_CUR_UP:
			// Si on est a la premiere ligne du buffer, on ne peux pas monter
			if(getCurLineBuffer(buffer) == 0){
				// On va au premier caractere de la ligne
		        setCurColBuffer(buffer, 0);
		    }else{ // Sinon, on monte notre curseur dans le buffer
			    setCurLineBuffer(buffer, getCurLineBuffer(buffer) - 1);
		    }
		    break;
		case BUFFER_CUR_DOWN:
			// Si on est a la derniere ligne du buffer, on ne peux pas descendre
			if(getCurLineBuffer(buffer) == getNbLineBuffer(buffer) - 1 ){
		        // On va donc au dernier charactere de la ligne
		        setCurColBuffer(buffer, strlen(getLineBuffer(buffer,getCurLineBuffer(buffer))));
		    }else{
		    	setCurLineBuffer(buffer, getCurLineBuffer(buffer) + 1);
		    }
		    break;

		case BUFFER_CUR_LEFT:
			// Si on est au debut de la ligne, on va au dernier caractere de la ligne au dessus
		    if(getCurColBuffer(buffer) == 0){
		        //sauf si on est a la premiere ligne
		        if(getCurLineBuffer(buffer) == 0){
		            copy_curPos_curSel(buffer);
		            return -1;
		        }
				setCurLineBuffer(buffer, getCurLineBuffer(buffer) - 1);
		        setCurColBuffer(buffer, strlen(getLineBuffer(buffer,getCurLineBuffer(buffer))));
		    }else{
		    	//Sinon, on le decale d'un caractere a gauche
		    	setCurColBuffer(buffer, getCurColBuffer(buffer) - 1);
		   	}
		   	break;
		case BUFFER_CUR_RIGHT:
			// Si on est a la fin de la ligne, on va au premier caractere de la ligne en dessus
			if(getCurColBuffer(buffer) == strlen(getLineBuffer(buffer, getCurLineBuffer(buffer)))){
				// sauf si on est a la derniere ligne
		        if(getCurLineBuffer(buffer) == getNbLineBuffer(buffer) - 1){
			        copy_curPos_curSel(buffer);
		            return -1;
		        }

		        setCurLineBuffer(buffer, getCurLineBuffer(buffer) + 1);
		        setCurColBuffer(buffer,0);
			}else{
				// Sinon, on le decale d'un caractere a droite
				setCurColBuffer(buffer, getCurColBuffer(buffer) + 1);
			}
			break;
		default:
			fprintf(stderr, "Direction incorrecte: buffer#moveCursorBuffer");
			return -1;
		}
	}

	copy_curPos_curSel(buffer);
	return 0;
}

int moveCursorPositionBuffer(struct Buffer* buffer, int ligne, int colonne){
	setCurLineBuffer(buffer, ligne);
	setCurColBuffer(buffer, colonne);
	copy_curPos_curSel(buffer);
	return 0;
}


int moveCursorSelectBuffer(struct Buffer* buffer, int direction, int distance){
	for(int i = 0 ; i < distance ; ++i){
		switch(direction){
		case BUFFER_CUR_UP:
			// Si on est a la premiere ligne du buffer, on ne peux pas monter
			if(getCurSelectLineBuffer(buffer) == 0){
				// On va au premier caractere de la ligne
		        setCurSelectColBuffer(buffer, 0);
		    }else{ // Sinon, on monte notre curseur dans le buffer
			    setCurSelectLineBuffer(buffer, getCurSelectLineBuffer(buffer) - 1);
		    }
		    break;
		case BUFFER_CUR_DOWN:
			// Si on est a la derniere ligne du buffer, on ne peux pas descendre
			if(getCurSelectLineBuffer(buffer) == getNbLineBuffer(buffer) - 1 ){
		        // On va donc au dernier charactere de la ligne
		        setCurSelectColBuffer(buffer, strlen(getLineBuffer(buffer,getCurSelectLineBuffer(buffer))));
		    }else{
		    	setCurSelectLineBuffer(buffer, getCurSelectLineBuffer(buffer) + 1);
		    }
		    break;

		case BUFFER_CUR_LEFT:
			// Si on est au debut de la ligne, on va au dernier caractere de la ligne au dessus
		    if(getCurSelectColBuffer(buffer) == 0){
		        //sauf si on est a la premiere ligne
		        if(getCurSelectLineBuffer(buffer) == 0){
		            return -1;
		        }
				setCurSelectLineBuffer(buffer, getCurSelectLineBuffer(buffer) - 1);
		        setCurSelectColBuffer(buffer, strlen(getLineBuffer(buffer,getCurSelectLineBuffer(buffer))));
		    }else{
		    	//Sinon, on le decale d'un caractere a gauche
		    	setCurSelectColBuffer(buffer, getCurSelectColBuffer(buffer) - 1);
		   	}
		   	break;
		case BUFFER_CUR_RIGHT:
			// Si on est a la fin de la ligne, on va au premier caractere de la ligne en dessus
			//printf("pos cur : %d\n", getCurSelectColBuffer(buffer));
			//printf("strlen : %d\n", strlen(getLineBuffer(buffer, getCurSelectLineBuffer(buffer))));
			if(getCurSelectColBuffer(buffer) == strlen(getLineBuffer(buffer, getCurSelectLineBuffer(buffer)))){
				// sauf si on est a la derniere ligne


		        if(getCurSelectLineBuffer(buffer) == getNbLineBuffer(buffer) - 1){

		            return -1;
		        }

		        setCurSelectLineBuffer(buffer, getCurSelectLineBuffer(buffer) + 1);
		        setCurSelectColBuffer(buffer,0);
			}else{
				// Sinon, on le decale d'un caractere a droite
				setCurSelectColBuffer(buffer, getCurSelectColBuffer(buffer) + 1);
			}
			break;
		default:
			fprintf(stderr, "Direction incorrecte: buffer#moveCursorSelectBuffer");
			return -1;
		}
	}
	return 0;
}



/**
 * Retire une ligne d'un buffer
 * La ligne du curseur est remonté si la ligne a supprimer est au dessus de
 * la position du curseur de sorte qu'elle reste "sur la meme ligne"
 *
 * buffer: le buffer dans lequel retirer une ligne
 *
 * noLine: le numero de la ligne a retirer
 *
 * Retour: 0 si tout se passe bien, -1 si la ligne a supprimer n'existe pas
 */
static int removeLineBuffer(struct Buffer* buffer, int noLine){
	// Si la ligne a supprimer est invalide
	if(noLine > buffer->nbLine || noLine < 0){
		return -1;
	}

	buffer->nbLine -= 1;
	free(getLineBuffer(buffer, noLine));

	memmove(
		buffer->lines+noLine,
		buffer->lines+noLine+1 ,
		(buffer->nbLine - noLine + 1)*sizeof(char*)
	);

	// Deplacement du curseur au cas ou elle est sous la suppression
	if(getCurLineBuffer(buffer) >= noLine){
		setCurLineBuffer(buffer, getCurLineBuffer(buffer) - 1);
	}

	return 0;
}


char* removeCharBuffer(struct Buffer* buffer, int nb){
	int nbSupp = 0; // Le nombre de chars supprimes
	char* chars_supp = calloc(1,1); // les chars supprimes

	// S'il existe une zone de selection alors elle doit etre supprimee
	if(selectZoneExistBuffer(buffer)){
		char* removed = removeSelectionZoneBuffer(buffer);
		++nbSupp; // On compte pour un caractere efface

		// Ajout dans chars_supp
		chars_supp = realloc(chars_supp, strlen(chars_supp) + strlen(removed) + 1);
		strcat(chars_supp, removed);

		free(removed);

	}


	while(nbSupp < nb){ // Tant qu'il faut supprimer des caracteres
		// Si on est au debut du buffer
		if(getCurLineBuffer(buffer) == 0 && getCurColBuffer(buffer) == 0){
			return chars_supp; // On ne peut plus rien supprimer
		}

		if(getCurColBuffer(buffer) == 0){ // Debut de la ligne

			// Reallocation de la ligne superieur
			reallocBuffer(
				buffer,
				getCurLineBuffer(buffer) - 1,
				strlen(getLineBuffer(buffer, getCurLineBuffer(buffer) - 1)) +
					strlen(getLineBuffer(buffer, getCurLineBuffer(buffer)))
					+ 1
			);

			// Position du curseur apres la suppression de la ligne
			int strlenLigne = strlen(getLineBuffer(buffer, getCurLineBuffer(buffer) - 1));

			// Copie de la ligne a la suite de la ligne du haut
			strcat(getLineBuffer(buffer, getCurLineBuffer(buffer) -1), getLineBuffer(buffer, getCurLineBuffer(buffer)));

			// Suppression de la ligne
			removeLineBuffer(buffer, getCurLineBuffer(buffer));

			// Deplacement des curseurs
			setCurColBuffer(buffer, strlenLigne);
			copy_curPos_curSel(buffer);

			++nbSupp;

			chars_supp = realloc(chars_supp, strlen(chars_supp) + 2);
			strcat(chars_supp, "\n");
		}else{// Si on n'est pas au debut d'une ligne
			int aSupp = nb - nbSupp; // le nombre de char qu'on peut supprimer en un coup au max

			if(getCurColBuffer(buffer) < aSupp){ // S'il n'y a pas assez de char sur la ligne
				aSupp = getCurColBuffer(buffer);
			}

			char* line = getLineBuffer(buffer, getCurLineBuffer(buffer));

			int fin = strlen(chars_supp) + aSupp ;

			chars_supp = realloc(chars_supp, fin+1);
			memcpy(chars_supp + strlen(chars_supp), line + getCurColBuffer(buffer) - aSupp, aSupp);
			chars_supp[fin] = '\0';

			// ecrasement de la partie a effacer
			memmove(
				line + getCurColBuffer(buffer) - aSupp,
				line + getCurColBuffer(buffer),
				strlen(line + getCurColBuffer(buffer)) + 1
			);

			// Diminution de la taille du buffer
			if(reallocBuffer(
						buffer, getCurLineBuffer(buffer),
						  strlen(getLineBuffer(buffer, getCurLineBuffer(buffer))) + 1
						) == NULL){
				return NULL;
			}

			// Deplacement des curseurs
			setCurColBuffer(buffer, getCurColBuffer(buffer) - aSupp);
			copy_curPos_curSel(buffer);

			nbSupp += aSupp;
		}
	}
	return chars_supp;
}


int copyBuffer(struct Buffer* buffer){
	char* zoneSelectionContent = getSelectionZoneBuffer(buffer);

	if(zoneSelectionContent == NULL){
		perror("Erreur Allocation mémoire : buffer#copy");
		return -1;
	}

	free(buffer->clipboard);
	buffer->clipboard = zoneSelectionContent;

	return strlen(buffer->clipboard);
}

int pasteBuffer(struct Buffer* buffer){
	return addTextBuffer(buffer, buffer->clipboard);
}


Position* findStringBuffer(Buffer* buffer, char* text){
	Position* position = newPosition();

	for(int i = 0 ; i < getNbLineBuffer(buffer); ++i){
		char* ligne = getLineBuffer(buffer, i);
		char* res = strstr(ligne, text);

		while(res != NULL){
			addPosition(position, i, res-ligne);
			res = strstr(res+1, text);
		}
	}

	return position;
}

int formatColumnWidthBuffer(struct Buffer* buffer){
	int no_line = 0;
	int position_init_line= getCurLineBuffer(buffer);
	int position_init_col = getCurColBuffer(buffer);

	// Tant qu'on n'est pas a la fin du fichier
	while(no_line < getNbLineBuffer(buffer)){
		char* line = getLineBuffer(buffer, no_line);

		// Tant qu'on peut compacter les lignes
		while(strlen(line) < buffer->auto_fill_width){
 			if(no_line + 1 >= getNbLineBuffer(buffer)){
				return 0;
			}

			char* removed;
			moveCursorPositionBuffer(buffer, no_line + 1, 0);

			// Si la ligne suivant n'est qu'un saut de ligne, on ne fait rien
			if(strlen(getLineBuffer(buffer, no_line + 1)) == 0){
				++no_line;
				continue;
			}

			if((removed = removeCharBuffer(buffer, 1)) == NULL){
				fprintf(stderr, "erreur lors de removeCharBuffer\n");
				return -1;
			}
			--position_init_line;
			free(removed);

			line = getLineBuffer(buffer, no_line);
		}

		int position = buffer->auto_fill_width;

		// On recherche le premier espace
		while(position > 0 && line[position - 1] != ' '){
			--position;
		}

		// Si on n'en trouve pas sur la ligne, on coupe le mot
		if(position == 0){
			position = buffer->auto_fill_width;
		}

		// On ajout un '\n' pour sauter une ligne
		moveCursorPositionBuffer(buffer, no_line, position);
		addTextBuffer(buffer, "\n");
		++position_init_line;

		moveCursorPositionBuffer(buffer, position_init_line, position_init_col - position);

		++no_line;
	}
	return 0;
}

int setFormatColumnWidthBuffer(Buffer* buffer, int actif){
	buffer->auto_fill = actif;
	return 0;
}

int setFormatColumnWidthSizeBuffer(Buffer* buffer, int value){
	buffer->auto_fill_width = value;
	return 0;
}
