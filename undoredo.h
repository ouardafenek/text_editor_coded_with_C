#ifndef UNDOREDO_H
#define UNDOREDO_H


/**
* Création des fichiers des modifications
* s'ils n'existent pas, sinon leur nettoyage 
*/ 
void initialisation_undoredo();
/**
 *  Ouverture du fichier des undo
 *  dans le mode donné
 */
int ouverture_fichier_undo(char * mode);

/**
 *  Ouverture du fichier des redo
 *  dans le mode donné
 */
int ouverture_fichier_redo(char * mode);
/**
 * Retrouver la denière modification faite dans les fichiers 
 */
char*  derniere_modification_undo();

char * derniere_modification_redo();

/**
 * Décomposer la modification qui était une chaine de caractère
 */
struct split* details_modification(char* modification);
/**
 * Se positionner dans les fichiers redo et undo a la ligne correspondant à
 * la modification courante
 */
void se_positionner();

/**
 * Annulation d'une modification ou annulation de l'annulation
 */
int undoRedo(Buffer* buffer, int undoOrRedo);

/**
 * Fermer les fichiers undo et redo
 */
void fermeture_fichier_modification();

/**
 * Ouvrir les fichiers undo et redo et se positionner à la
 * ligne de lla modification courante
 */
void positionnement_dans_fichier_modification();

/**
 * Vérifie si il y a une zone de selection dans le buffer
 * Retour:
 * 1 s'il y a une zone de selection,
 * 0 sinon
 */
int si_selection();
/**
 * Inserer le caractère saisi par l'utlisateur 
 * Ecrire cette modification dans le fichier des redo 
 * Ecrire son inverse dans le fichier des undo 
 * paramètres : la commande saisie par l'utlisateur 
 */ 

int inserer(Buffer* buffer, char entree);
/**
 * Coller ce qu'il y a dans le press papier du buffer 
 * à la position du ciurseur 
 * Ecrire cette modification dans le fichier des redo 
 * Ecrire son inverse dans les fichiers des undo 
 */ 
int coller(Buffer* buffer);
/**
 * Supprimer ce qu'il y a dans le press papier du buffer 
 * à la position du ciurseur 
 * Ecrire cette modification dans le fichier des redo 
 * Ecrire son inverse dans les fichiers des undo 
 */ 
int supprimer(Buffer* buffer);
/**
 * Supprimer un caractère en arrière à partir de la position courante
 *
 */
int suprimerBack(Buffer* buffer); 
#endif