#ifndef EDITEUR_H
#define EDITEUR_H

/**
 * Demander à l'utilisateur de saisir le nom du fichier à charger
 * Retour: Le nom saisi
 */
void demande_nom_fichier_chargement(); 
/**
 * Permet de sortir du programme en cas d'Erreur
 */
void sortie_erreur();

/**
 * teste si on a deja sauvegarder le fichier, sinon demande un noms pour la sauvegarde
 */
int aDejaSauvegarder();


/**
    demande a l'utilisateur si il veux sauvegarder avant de quitter
    comparer simplement le premier charactere avec "o" ou "n"

    return 0 si il veux sauvegarder, 1 si il ne veux pas et -1 sinon sont n'entrer
                ne commence ni par "o" ou "n"
*/
int veuxSauvegarder();
/**
 * Sort du programme quand l'utilisateur en fait la demande
 *
 * Retour: 0 Si on sort correctement, 1 si l'utilisateur ne veux pas sortir
 */
int sortie();
/**
 * Sauvegarde le buffer dans le fichier nom_du_fichier, sera initialiser si ce
 * n'est pas le cas
 *
 * Retour: 1 Si la sauvegarde s'est bien passer
 */
int sauvegarde_editeur();



/**
 * A partir de la position y (ligne) du curseur dans le buffer
 * trouver sa position dans la console
 */
int getCurLineScr (Buffer* buffer);

/**
 * A partir de la position x du curseur dans le buffer
 * trouver sa position sur la console
 */
int getCurColScr(Buffer* buffer);


/**
 * A partir de la position y (ligne) du curseur dans le buffer
 * trouver sa position dans la console
 */


int getCurLineSelectScr (Buffer* buffer);


/**
 * A partir de la position x du curseur dans le buffer
 * trouver sa position sur la console
 */


int getCurColSelectScr(Buffer* buffer);

/**
 * Recupere le buffer et se contente de l'afficher dans la fenetre utilisateur
 */

int affiche_buffer();

/**
 * Appeler dans le cas ou l'utilisateur redimensionne la fenetre, se contente de
 * re-afficher le buffer
 *
 * Retour: 1 si tout c'est bien passe, -1 sinon
 */


int redimensionnement();

/**
 * A partir du char c qui code la direction, applique au curseur dans le buffer
 * sa nouvelle bonne position
 *
 * c: le charactere qui code la fleche directionnel entrer par l'utilisateur
 *
 * Retour: Un nombre positif si le deplacement est possible, -1 sinon
 */


int deplacement_curseur_dans_buffer(char * c);



/**
 * Gere le deplacement et l'affiche du curseur dans le buffer
 *
 * c: le charactere entrer par l'utilisateur
 *
 * Retour: 1 si tous ce passe bien, -1 sinon
 */


int deplacement_curseur(char * c);


/**
 * Traite le charactere entrer par l'utilisateur, l'ajout au buffer, et affiche
 * le resultat
 * L'idee aussi de cette fonction est de separe si besoin la gestion des
 * differents characteres, ie \t \n abcd... etc
 *
 * c: le charactere entrer par l'utilisateur
 *
 * Retour: 1 si tous ce passe bien, -1 sinon
 */


int traitement_char(char c);



/**
 * Retourne une constante de direction selon le caractère saisi
 */


int direction(char c);

/**
 * Charger un fichier à ppartir d'un buffer déjà existant 
 * Cette fonction enregistre d'abord le contenu du buffer 
 */ 

int charger();



/**
    Affiche le buffer, puis pour chaque partie qui fait partie de la selection de
    find le print en rouge

    return 1 si tous s'est bien passé
*/

int affiche_buffer_find(Position * p, char * toFind);

/**
    Recuperer grace a findStringBuffer du module buffer les positions des elements
    correspondant a toFind et affiche sur la console

    return 1 si tous c'est bien passé
*/
int find_select(char * toFind);


/**
    Fonction qui est appeler si la commande find est active sans zone de selection
    qui va d'abord demander a l'utilisateur de rentrer des char a finder et
    ensuite le laisser chercher dans son document avec find_select(char * toFind)
    La chaine toFind sera ce qu'il a remplie
*/
int find();
/**
 * Demander le num du raccourci à modifier (chaque raccourci)
 */
char * demande_num_raccourci();


/**
 *  Tester si un raccourci est déjà utilisé dans le fichier de configuration 
 *  Retour : 1 si déjà utilisé, 0 sinon 
 */

int dejaUtilise(char c);

/**
 * Demande le nouveau raccourci, vérifie qu'il s'agit d'une lettre
 * et vérifie qu'il n'est pass déjà exploité
 */
char demande_raccourci();

/** 
 *  Fonction auxiliaire qui fera la correspendance entre l'ordre des raccourcis
 *  dans le fichier des configuration et de l'option qu'ils représentent 
 */ 
char * correspondance(int ligne);


 /**
  * Modification des raccourcis
  */
int modifier(); 

/**
 * Choisir une option et exécuter dans le menu 
 * Executer la fonction correspondante ensuite
 */ 
int choisir(); 

    /**
        Fonction qui traite les entrès de l'utilisateur, en fonction de leur type
            et qui dans chaque cas invoque la bonne methode pour le programme

        int entrer : le type de l'entrer QUIT, INSERT, COPY, CUT ...

        struct command * entrer_user :  la structure qui contient le charactere
                                        entrer par l'user

        return -1 en cas d'echec, 0 si QUIT et 1 sinon
    */

int traitement_entrer(int entrer, struct command * entrer_user);

/** 
 * Boucle d'intéraction entre le buffer et l'utilisateur
 */ 
void boucle_utilisateur(); 

int initialisation(int flag,char * nomDeFichier);


#endif
