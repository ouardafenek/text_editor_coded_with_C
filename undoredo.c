#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include "parser.h"
#include "buffer.h"
#include "undoredo.h"

char * nom_redo = "redo.txt";
FILE * redo = NULL;
char * nom_undo = "undo.txt";
FILE * undo = NULL;


int num_modification = 0;
int nb_undo =0;


/*
# format des commandes enregistrées dans le undo_redo
# INSERT: [ligne] [colonne] [caractère]
# PASTE: [ligne] [colonne] [texte]
# DELETE: [buffer_cur_ligne] [buffer_cur_colonne] [select_cur_ligne] [select_col_colonne] [texte]
# DELETEBACK: [ligne] [colonne]
*/



/**
* Création des fichiers des modifications
* s'ils n'existent pas, sinon leur nettoyage
*/
void initialisation_undoredo(){
    undo=fopen(nom_undo,"w");
    if (undo!=NULL) fclose(undo);

    redo=fopen(nom_redo,"w");
    if (redo!=NULL) fclose(redo);
}


/**
 *  Ouverture du fichier des undo
 *  dans le mode donné
 */
int ouverture_fichier_undo(char * mode){
    undo = fopen(nom_undo, mode);
    if(undo==NULL){
        printf("Impossible d'ouvrir le fichier de sauvegarde des do");
        return -1;
    }
    return 0;
}


/**
 *  Ouverture du fichier des redo
 *  dans le mode donné
 */
int ouverture_fichier_redo(char * mode){
    redo = fopen(nom_redo, mode);
    if(redo==NULL){
        printf("Impossible d'ouvrir le fichier de sauvegarde des do");
        return -1;
    }
    return 0;
}
/**
 * Retrouver la denière modification faite dans les fichiers undo
 */
char*  derniere_modification_undo (){
   undo = fopen(nom_undo,"r");
   int i = 0 ;
   char * s = malloc (sizeof(char) * 256);
   while(i<num_modification){
    fgets(s,256,undo);
    i++;
   }
   fclose(undo);
   num_modification --;
   if(num_modification<0) num_modification=0;
   return s;

}

char * derniere_modification_redo(){
   redo = fopen(nom_redo,"r");
   int i = 0 ;
   num_modification ++;
   char * s = malloc (sizeof(char) * 256);
   while(i<num_modification && s!=NULL){
    fgets(s,256,redo);
    i++;
   }
   fclose(redo);
   return s;
}

/**
 * Décomposer la modification qui était une chaine de caractère
 */
struct split* details_modification(char* modification){
    char * modification_copy = malloc(sizeof(char)*256);
    strcpy(modification_copy,modification);
    free(modification);
    struct split* s =  message_parser(modification_copy);
    if(s->nb_message<3) return NULL ;
    return s;
}
/**
 * Se positionner dans les fichiers redo et undo a la ligne correspondant à
 * la modification courante
 */
void se_positionner(){
    int i = 0 ;
    char *s = malloc (sizeof(char)*256);
    while(i<num_modification){
        fgets(s,256,undo);
        fgets(s,256,redo);
        i++;
    }
    free(s);
}

/**
 * Annulation d'une modification ou annulation de l'annulation
 */
int undoRedo(Buffer * buffer, int undoOrRedo){
    char *s = NULL;
    if(undoOrRedo==-1){
    	if (num_modification==0) return -1;
    	nb_undo++;
        s = derniere_modification_undo();
    }
    else{
    	if(nb_undo<=0) return -1;
    	nb_undo--;
        s = derniere_modification_redo();
    }
    //test d'erreur
    if (s==NULL) return -1;

    struct split* modification = details_modification(s);
    //test d'erreur
    if(modification==NULL) return -1;

    if(strcmp(modification->messages[0],"INSERT:")==0){
        moveCursorPositionBuffer(buffer,atoi(modification->messages[1]),
            atoi(modification->messages[2]));
        moveCursorSelectBuffer(buffer, atoi(modification->messages[1]),
            atoi(modification->messages[2]));
        addTextBuffer(buffer, modification->messages[3]);
    }

    else if(strcmp(modification->messages[0],"DELETEBACK:")==0){
        moveCursorPositionBuffer(buffer,atoi(modification->messages[1]),
            atoi(modification->messages[2]));
        moveCursorSelectBuffer(buffer, atoi(modification->messages[1]),
            atoi(modification->messages[2]));
        removeCharBuffer(buffer,1);
    }

    else if(strcmp(modification->messages[0],"DELETE:")==0){
        moveCursorPositionBuffer(buffer,atoi(modification->messages[1]),
            atoi(modification->messages[2]));
        moveCursorSelectBuffer(buffer, atoi(modification->messages[1]),
            atoi(modification->messages[2]));
        removeCharBuffer(buffer,atoi(modification->messages[3]));
    }

    else if(strcmp(modification->messages[0],"PASTE:")==0){
        moveCursorPositionBuffer(buffer,atoi(modification->messages[1]),
            atoi(modification->messages[2]));
        addTextBuffer(buffer, modification->messages[3]);
    }
    else {
        free_split(modification);
        return -1;
    }

    free_split(modification);
    return 0;
}

/**
 * Fermer les fichiers undo et redo
 */
void fermeture_fichier_modification(){
  fclose(undo);
  fclose(redo);

}

/**
 * Ouvrir les fichiers undo et redo et se positionner à la
 * ligne de lla modification courante
 */
void positionnement_dans_fichier_modification(){
    if(ouverture_fichier_undo("r+")==-1) exit(0);
    if(ouverture_fichier_redo("r+")==-1) exit(0);
    se_positionner();
}

/**
 * Vérifie si il y a une zone de selection dans le buffer
 * Retour:
 * 1 s'il y a une zone de selection,
 * 0 sinon
 */
int si_selection(Buffer* buffer){
    return retrouve_premier_buffer(buffer)!=0;
}
/**
 * Inserer le caractère saisi par l'utlisateur
 * Ecrire cette modification dans le fichier des redo
 * Ecrire son inverse dans le fichier des undo
 * paramètres : la commande saisie par l'utlisateur
 */

int inserer(Buffer * buffer,char entree){
    char *c;
    positionnement_dans_fichier_modification();
    if(si_selection(buffer)){
       fprintf(redo, "DELETE:!>%d!>%d!>%lu!>\n",getCurLineBuffer(buffer), 
            getCurColBuffer(buffer),strlen(c = removeCharBuffer(buffer,0)));
       num_modification++;
       fprintf(undo, "PASTE:!>%d!>%d!>%s!>\n",getCurLineBuffer(buffer),getCurColBuffer(buffer),c);
    }
    fprintf(redo, "INSERT:!>%d!>%d!>%c!>\n",getCurLineBuffer(buffer),getCurColBuffer(buffer),
        entree);
    //int r = traitement_char(entree);
    char * tmp = calloc(2,sizeof(char));
    tmp[0]=entree;
            // Puis ajouter le tableau cree au buffers
    addTextBuffer(buffer,tmp);
    free(tmp);
    num_modification++;
    fprintf(undo, "DELETEBACK:!>%d!>%d!>%c!>\n",getCurLineBuffer(buffer),getCurColBuffer(buffer),
        entree);
    fermeture_fichier_modification();

    return 1;
}
/**
 * Coller ce qu'il y a dans le press papier du buffer
 * à la position du ciurseur
 * Ecrire cette modification dans le fichier des redo
 * Ecrire son inverse dans les fichiers des undo
 */
int coller(Buffer * buffer){
    positionnement_dans_fichier_modification();
    fprintf(redo, "PASTE:!>%d!>%d!>%s!>\n",getCurLineBuffer(buffer),getCurColBuffer(buffer),
        getClipBoardBuffer(buffer));
    num_modification++;
    int r = pasteBuffer(buffer);
    fprintf(undo, "DELETE:!>%d!>%d!>%lu!>\n",getCurLineBuffer(buffer),getCurColBuffer(buffer),
        strlen(getClipBoardBuffer(buffer)));
    fermeture_fichier_modification();
    return r;
}
/**
 * Supprimer ce qu'il y a dans le press papier du buffer
 * à la position du ciurseur
 * Ecrire cette modification dans le fichier des redo
 * Ecrire son inverse dans les fichiers des undo
 */
int supprimer(Buffer * buffer){
    char *c;
    positionnement_dans_fichier_modification();
    fprintf(redo, "DELETE:!>%d!>%d!>%lu!>\n",getCurLineBuffer(buffer),getCurColBuffer(buffer),
        strlen(c = removeCharBuffer(buffer,0)));
    fprintf(undo,"PASTE:!>%d!>%d!>%s!>\n",getCurLineBuffer(buffer),getCurColBuffer(buffer),c);
    num_modification++;
    fermeture_fichier_modification();
    return 1;
}
/**
 * Supprimer un caractère en arrière à partir de la position courante
 *
 */
int suprimerBack(Buffer * buffer){
    char * c;
    positionnement_dans_fichier_modification();
    fprintf(redo, "DELETEBACK:!>%d!>%d!>\n",getCurLineBuffer(buffer),getCurColBuffer(buffer));
    c = removeCharBuffer(buffer,1);
    fprintf(undo, "INSERT:!>%d!>%d!>%s!>\n",getCurLineBuffer(buffer),getCurColBuffer(buffer),c);
    num_modification++;
    fermeture_fichier_modification();
    return 1;
}
