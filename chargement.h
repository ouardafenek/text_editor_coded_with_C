#ifndef CHARGEMENT_H
#define CHARGEMENT_H



/**
	chargement du fichier nom_fichier dans un buffer
	ce buffer est passé en paramètres->passage par variables
	retour :
		0 -> succès
		-1-> en cas d'echec, soit dans l'ouverture du fichier ou dans l'allocation de mémoire

	exemple d'appel pour la fonction :
		char * buf_fichier =NULL ;
		char * nom_fichier = "text.txt" ;
		chargement(nom_fichier , &buf_fichier) ;
*/
int chargement(char ** buf_fichier, char * nom_fichier);

/**
*
*   Procedure a voire pour la sauvegarde ??
*
*/
int sauvegarde(char * buffer, char * nom_fichier);



#endif
