#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/**
 * Chargement du contenue du fichier nom_fichier dans un buffer buf_fichier
 *
 * buf_fichier: L'adresse du buffer a remplir avec les donnees du fichier
 *
 * nom_fichier: Le fichier dans lequel on charge les donnees pour le buffer
 *
 * Retour: 0 si tous ce passe bien, -1 sinon
 */
int chargement (char ** buf_fichier, char * nom_fichier){
	// On charge le descripteur de fichier dans f
	FILE * f = fopen(nom_fichier,"r");
	// Si f est differt de NULL, c'est que la creation du descripteur a reussi
	if (f!=NULL){
		fseek (f, 0 , SEEK_END) ;
		int taille = ftell(f) ;
		rewind(f) ;
		*buf_fichier = malloc (sizeof (char) * (taille+1));
		if (*buf_fichier==NULL){
			fprintf(stderr, "Allocation de mémoire\n");
			return -1 ;
		}
		// Si le nombre de charactere lu dans le fichier differt du nombre de charactere
		// total dans le fichier, alors il y a une erreur
		if(fread(*buf_fichier,sizeof(char),taille,f)!=taille){
			fprintf(stderr, "Chargement, chargement, la lecture du descripteur dans le buffer a echoue\n");
			return -1;
		}

		// Si la fermeture du fichier renvoie EOF c'est qu'elle a echoue
		if(fclose(f)==EOF){
			fprintf(stderr, "Chargement, chargement, la fermeture du descripteur a echoue\n");
			return -1;
		}

		// Sinon tous c'est bien passe
		return 0 ;
	}
	fprintf(stderr, "Chargement, chargement, la creation du descripteur a echoue\n");
	return -1 ;
}

/**
 * Sauvegarde le buffer dans un fichier
 *
 * buffer: Le buffer a sauvegarder
 *
 * nom_fichier: Le fichier dans lequel sauvegarder le buffer
 *
 * Retour: 0 si tous ce passe bien, -1 sinon
 */
int sauvegarde (char * buffer, char * nom_fichier){
	// On charge le descripteur de fichier dans f
	FILE * f = fopen(nom_fichier,"w");
	// Si f est differt de NULL, c'est que la creation du descripteur a reussi
	if (f!=NULL){
		// Si le nombre de charactere ecrit dans le fichier differt du nombre de charactere
		// dans le buffer alors il y a une erreur
		if(fwrite(buffer,sizeof(char),strlen(buffer),f)!=strlen(buffer)){
			fprintf(stderr, "Chargement, sauvegarde, l'ecriture du buffer dans le descripteur a echoue\n");
			return -1;
		}

		// Si la fermeture du fichier renvoie EOF c'est qu'elle a echoue
		if(fclose(f)==EOF){
			fprintf(stderr, "Chargement, sauvegarde, la fermeture du descripteur a echoue\n");
			return -1;
		}

		// Sinon tous c'est bien passe
		return 0;
	}
	fprintf(stderr, "Chargement, sauvegarde, la creation du descripteur a echoue\n");
	return -1;
}
