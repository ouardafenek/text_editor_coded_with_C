#include "position.h"

#ifndef BUFFER_H
#define BUFFER_H

#define BUFFER_CUR_UP 1
#define BUFFER_CUR_DOWN 2
#define BUFFER_CUR_LEFT 3
#define BUFFER_CUR_RIGHT 4

typedef struct Buffer Buffer;

/**
 * Crée un buffer
 *
 * nbLine: le nombre de ligne max initial
 * 
 * Retour: un nouveau buffer
 */
struct Buffer* newBuffer(int);	

/**
 * Recupere le nombre de ligne d'un buffer
 * 
 * buffer: le buffer 
 *
 * Retour: un entier representant le nombre de ligne
 */
int getNbLineBuffer(Buffer*);


/**
 * Retourne ce qui est dans le press papier du buffer
 *
 * buffer : le buffer
 */ 
char * getClipBoardBuffer(struct Buffer* buffer);

/**
 * Libere les ressources utilisees pour le buffer
 *
 * buffer: le buffer a liberer
 */
void freeBuffer(Buffer*);

/**
 * Recupère une ligne d'un buffer 
 *
 * buffer: le buffer dans lequel recuperer une ligne
 *
 * noLine: le numero de la ligne a recuperer
 *
 * Retour: la ligne specifiee du buffer indiquée, NULL en cas d'echec
 */
char* getLineBuffer(Buffer*, int);



/**
 * Permet l'ajout d'une chaine de caractere a la position du curseur courant, 
 * L'ajout gere les saut de lignes en creant une nouvelle ligne dans le buffer
 * Les caracters apres le curseurs sont deplace vers la droite (insertion du texte)
 * S'il y a une zone de selection definie, alors il sera ecrase par le nouveau texte 
 * et la zone de selection est supprime
 *
 * buffer: le buffer
 *
 * txt: le texte a ajouter
 * 
 * Retour: la longueur du texte recu
 */
int addTextBuffer(Buffer*, char*);

/**
 * Permet de recuperer tout le texte d'un buffer
 * La memoire utilise par la  chaine de caractere du retour doit etre libere
 * 
 * buffer: le buffer dans lequel recuperer les informations
 * 
 * Retour: une chaine de caracteres representant les contenus des lignes du buffer
 * Ou NULL en cas de probleme
 */
char* getTextBuffer(Buffer*);

/**
 * Permet de récupérer une portion du buffer
 * La mémoire utilisé par la chaine de caracteres du retour doit etre libere
 * Erreur si le debut est negatif, la fin est superieur au nombre de ligne dans 
 * le buffer et su le debut est apres la fin
 * 
 * buffer: le buffer dans lequel recuperer le texte
 * 
 * debut: no de la premiere ligne
 * 
 * fin: no de la derniere ligne (inclus)
 * 
 * Retour: une chaine de caractere representant le contenue des lignes du buffer
 * demande avec les informations, NULL en cas de probleme
 */
char* getTextPartielBuffer(Buffer*, int debut, int fin);

/**
 * Recupere la ligne du curseur de position d'un buffer
 * 
 * buffer: le buffer
 *
 * Retour: la ligne du curseur de position
 */
int getCurLineBuffer(Buffer*);

/**
 * Recupere la colonne du curseur de position
 * 
 * buffer: le buffer
 *
 * Retour: la colonne du curseur de position
 */
int getCurColBuffer(Buffer*);

/**
 * Recupere la ligne du curseur de selection
 * 
 * buffer: le buffer
 *
 * Retour: la ligne du curseur de selection
 */
int getCurSelectLineBuffer(Buffer*);

/**
 * Recupere la ligne du curseur de selection
 * 
 * buffer: le buffer
 *
 * Retour: la ligne du curseur de selection
 */
int getCurSelectColBuffer(Buffer*);

/**
 * Permet de retrouver le curseur representant le debut de la zone de selection d'un buffer
 *
 * buffer: le buffer dans lequel compter
 * 
 * Retour : un entier negatif si le curseur position est en premier, 
 * 		positif si le curseur de selection est en premier, 0 Si les deux sont confondus
 */
int retrouve_premier_buffer(Buffer*);

/**
 * Deplace de curseur dans une certaint direction pour une certaint distance
 * Le déplacement deplace les deux curseurs, le curseur de selection sera a la meme place que le curseur de position
 * 
 * Buffer, le buffer dans lequel deplacer les curseurs
 * 
 * direction: la direction (Utiliser les defines)
 * 
 * distance: le nombre de case a parcourir
 *
 * Retour: 0 si le deplacement est effectuee, -1 sinon
 */
int moveCursorBuffer(Buffer*, int direction, int distance);

/**
 * Deplace le curseur a une position spécifique
 *
 * buffer : le buffer dans lequel deplacer le curseur
 *
 * ligne : la ligne où doit se déplacer le curseur
 *
 * colonne : la colonne où doit se deplacer le curseur
 *
 * Retour : 0 si le déplacement est effectuée, -1 en cas d'erreur
 */
int moveCursorPositionBuffer(Buffer*, int, int);

/**
 * Deplace le curseur de selection d'un buffer dans une certaint direction sur une certaint distance.
 * Le curseur de position n'est pas deplace
 * 
 * buffer: le buffer
 * 
 * direction: la direction (utiliser les defines)
 * 
 * distance: le nombre decase a parcourir
 * 
 * Retour: 0 si le deplacement est effectuee, -1 sinon
 */
int moveCursorSelectBuffer(Buffer*, int direction, int distance);

/**
 * Retire un certaint nombre de caracteres au niveau du curseur dans un buffer
 *
 * buffer: le buffer dans lequel retirer des caracteres
 * 
 * nb: le nombre de caracteres a retirer
 * 
 * retour le nombre de caractere effectivement retiré
 */
char* removeCharBuffer(Buffer*, int);

/**
 * Copie la zone de selection dans le presse papier interne
 *
 * buffer: le buffer dans le quel copier le texte
 *
 * Retour: le nombre de caractere copié (-1 en cas d'erreur)
 */
int copyBuffer(Buffer*);

/**
 * Colle le presse papier à la position du curseur
 *
 * Buffer: Le buffer dans lequel effectuer les opérations
 *
 * Retour: le nombre de caractere collées
 */
int pasteBuffer(Buffer*);

/**
 * Récupère le contenue de la zone de selection
 * 
 * buffer/ le buffer dans lequel chercher
 *
 * Retour: la chaine de caracteres correspondant au contenue de la zone de selection
 */
char* getSelectionZoneBuffer(Buffer*);

/**
 * Recherche toutes les occurences d'une chaine de caracteres dans un buffer
 *
 * buffer: le buffer dans lequel rechercher
 *
 * text : la chaine de caractere a chercher
 *
 * Retour un tableau de Positions correspondant a la ligne et a la colonne d'un resulat
 */
 Position* findStringBuffer(Buffer*, char*);
 
/**
 * Format un buffer pour que toutes les lignes ne dépasser pas un certaint nombre de caracteres
 *
 * buffer : le buffer dans lequel effectuer les opérations
 *
 * Retour : 0 si la formatage est bien effectuée, -1 en cas d'erreur
 */
int formatColumnWidthBuffer(Buffer*);

/**
 * Active ou desactive auto-fill
 * 
 * buffer: le buffer dans le quel faire mes modifications
 *
 * actif: 0 si c'est inactif, actif sinon
 *
 * Retour: 0
 */
int setFormatColumnWidthBuffer(Buffer*, int);

/**
 * Modifie la largeur du formatage 
 *
 * buffer, le buffer dans lequel faire les modification
 * 
 * size: la largeur des colonnes
 */
int setFormatColumnWidthSizeBuffer(Buffer*, int);

#endif
