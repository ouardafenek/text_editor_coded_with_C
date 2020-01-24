#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include "buffer.h"
#include "requette.h"
#include "affichage.h"
#include "chargement.h"
#include "undoredo.h"

Buffer * buffer;
char * nom_du_fichier=NULL;
int line_wrapping = 1;


int nb_de_modification_sans_sauvagarde = 0;

// Declaration initiale
int affiche_buffer();
int traitement_entrer(int entrer, struct command * entrer_user);

/**
 * Demander à l'utilisateur de saisir le nom du fichier à charger
 * Retour: Le nom saisi
 */
void demande_nom_fichier_chargement(){
    print_position_curseur(getCurLineBuffer(buffer),getCurColBuffer(buffer));
    print_line_buffer(" : quel noms pour votre fichier a charger : ",1);
    refresh();
    nom_du_fichier = demande_nom_fichier();
}

/**
 * Permet de sortir du programme en cas d'Erreur
 */
void sortie_erreur(){
    quitter_affichage();
    freeBuffer(buffer);
    exit(0);
}

/**
 * teste si on a deja sauvegarder le fichier, sinon demande un noms pour la sauvegarde
 */
int aDejaSauvegarder(){
    if (nom_du_fichier==NULL) {
        print_position_curseur(getCurLineBuffer(buffer),getCurColBuffer(buffer));
        print_line_buffer(" : quel noms pour votre fichier a sauvegarder : ",1);
        refresh();
        nom_du_fichier = demande_nom_fichier();
    }
    return 1;
}


/**
    demande a l'utilisateur si il veux sauvegarder avant de quitter
    comparer simplement le premier charactere avec "o" ou "n"

    return 0 si il veux sauvegarder, 1 si il ne veux pas et -1 sinon sont n'entrer
                ne commence ni par "o" ou "n"
*/
int veuxSauvegarder(){
    print_position_curseur(getCurLineBuffer(buffer),getCurColBuffer(buffer));
    print_line_buffer(" : Voulez-vous sauvegarder avant de quitter ? o/n ",1);
    refresh();
    char * reponse = demande_nom_fichier();
    if (strncmp(reponse,"o",1)==0) {
        free(reponse);
        return 0;
    }
    if (strncmp(reponse,"n",1)==0) {
        free(reponse);
        return 1;
    }
    free(reponse);
    return -1;
}

/**
 * Sort du programme quand l'utilisateur en fait la demande
 *
 * Retour: 0 Si on sort correctement, 1 si l'utilisateur ne veux pas sortir
 */
int sortie(){
    switch (veuxSauvegarder()) {
        case 0: // veux sauvegarder
            aDejaSauvegarder();
            char * a_sauvegarder = getTextBuffer(buffer);
            sauvegarde(a_sauvegarder,nom_du_fichier);
            free(a_sauvegarder);
            return 0;
            break;
        case 1: // ne veux pas sauvegarder
            return 0;
            break;

        case -1: // ne veux pas quitter
            affiche_buffer();
            return 1;
            break;
    }
    return 0;
}

/**
 * Sauvegarde le buffer dans le fichier nom_du_fichier, sera initialiser si ce
 * n'est pas le cas
 *
 * Retour: 1 Si la sauvegarde s'est bien passer
 */
int sauvegarde_editeur(){
    //
    aDejaSauvegarder();
    sauvegarde (getTextBuffer(buffer),  nom_du_fichier);
    affiche_buffer();
    return 1;
}

/**
 * A partir d'une ligne dans le buffer
 * trouver sa position dans la console
 */
int getCurLineScr (Buffer* buffern, int index_ligne){
    int line =index_ligne;
    int longueur_fenetre ;
    int largeur_fenetre;
    int premier_line = taille_fenetre(&longueur_fenetre, &largeur_fenetre);
    int i = premier_line;
    while(i<index_ligne){
        line +=  strlen(getLineBuffer(buffer,i))/ largeur_fenetre;
        i++;
    }
    line+= getCurColBuffer(buffer)/largeur_fenetre;
    return line;
}


/**
 * A partir de la position x du curseur dans le buffer
 * trouver sa position sur la console
 */
int getCurColScr(Buffer* buffer){
    int longueur_fenetre ;
    int largeur_fenetre;
    taille_fenetre(&longueur_fenetre, &largeur_fenetre);
    int col = getCurColBuffer(buffer) ;
    if (col >= largeur_fenetre)
        col = col % largeur_fenetre;
    return col;
}


/**
 * A partir de la position y (ligne) du curseur dans le buffer
 * trouver sa position dans la console
 */


int getCurLineSelectScr (Buffer* buffer){
    int line = getCurLineBuffer(buffer) ;
    int longueur_fenetre ;
    int largeur_fenetre;
    int premier_line = taille_fenetre(&longueur_fenetre, &largeur_fenetre);
    int i = premier_line;
    while(i<getCurSelectLineBuffer(buffer)){
        line +=  strlen(getLineBuffer(buffer,i))/ largeur_fenetre;
        i++;
    }
    line+= getCurSelectColBuffer(buffer)/largeur_fenetre;
    return line;
}


/**
 * A partir de la position x du curseur dans le buffer
 * trouver sa position sur la console
 */


int getCurColSelectScr(Buffer* buffer){
    int longueur_fenetre ;
    int largeur_fenetre;
    taille_fenetre(&longueur_fenetre, &largeur_fenetre);
    int col = getCurSelectColBuffer(buffer) ;
    if (col >= largeur_fenetre)
        col = col % largeur_fenetre;
    return col;
}


/**
 * Recupere le buffer et se contente de l'afficher dans la fenetre utilisateur
 */
int affiche_buffer(){
    clear_window(); // on efface tous
    int cols;
    int lines;
    int premier_line = taille_fenetre(&lines,&cols); // on recupere les info utils
    //int i = premier_line;
    char * line;
    move_print_line(0,0);
    // On affiche le texte une premiere fois ligne par ligne
    for (int i  = premier_line ;i<premier_line+LINES-1;i++){
        line = getLineBuffer(buffer,i);
        if(i>=getNbLineBuffer(buffer) || line == NULL || (getNbLineBuffer(buffer)==1 && strcmp(line,"")==0)){
	         break;
         }else{
             print_line_buffer(line,line_wrapping);
         }
    }


    //maintenant on recuperer le texte selectionner
    char * lines_select=getSelectionZoneBuffer(buffer);

    // si en devant le curseur principale
    if (retrouve_premier_buffer(buffer)<0) {
        color_select_zone(getCurLineScr(buffer,getCurLineBuffer(buffer)),getCurColScr(buffer),lines_select,1);
    }
    // si elle est denière
    else if(retrouve_premier_buffer(buffer)>0){
        color_select_zone(getCurLineSelectScr(buffer),getCurColSelectScr(buffer),lines_select,1);
    }


    print_position_curseur(getCurLineBuffer(buffer),getCurColBuffer(buffer));
    deplacer(getCurLineScr(buffer,getCurLineBuffer(buffer)),getCurColScr(buffer));
    refresh();

    return 1;
}



/**
 * Appeler dans le cas ou l'utilisateur redimensionne la fenetre, se contente de
 * re-afficher le buffer
 *
 * Retour: 1 si tout c'est bien passe, -1 sinon
 */
int redimensionnement(){
    // On va simplement re-afficher le biffer
    affiche_buffer();
    // Et reaffichier le curseur au bonne emplacement
    print_position_curseur(getCurLineBuffer(buffer),getCurColBuffer(buffer));
    deplacer(getCurLineScr(buffer,getCurLineBuffer(buffer)),getCurColScr(buffer));
    return 1;
}


/**
 * A partir du char c qui code la direction, applique au curseur dans le buffer
 * sa nouvelle bonne position
 *
 * c: le charactere qui code la fleche directionnel entrer par l'utilisateur
 *
 * Retour: Un nombre positif si le deplacement est possible, -1 sinon
 */
int deplacement_curseur_dans_buffer(char * c){
    int largeur_fenetre, longueur_fenetre;
    taille_fenetre(&longueur_fenetre,&largeur_fenetre);
    // On regarde la bonne direction de notre deplacement
    switch (*c) {
        case 117: //up
            if(getCurColBuffer(buffer)>largeur_fenetre){
                return moveCursorBuffer(buffer, BUFFER_CUR_LEFT, largeur_fenetre );
            }
            else{
                return moveCursorBuffer(buffer, BUFFER_CUR_UP,1);
             }
            break;
        case 114: //right
            return moveCursorBuffer(buffer, BUFFER_CUR_RIGHT, 1);
            break;
        case 108: //left
            return moveCursorBuffer(buffer, BUFFER_CUR_LEFT, 1);
            break;
        case 100: //down
            return moveCursorBuffer(buffer, BUFFER_CUR_DOWN,1) ;
            break;
    }
  	return -1;
}



/**
 * Gere le deplacement et l'affiche du curseur dans le buffer
 *
 * c: le charactere entrer par l'utilisateur
 *
 * Retour: 1 si tous ce passe bien, -1 sinon
 */
int deplacement_curseur(char * c){
    // On regarde d'abord si il y a une zone de selection, si c'est le cas et que l'on deplace
    // seulement le curseur, alors il faudra re-afficher le buffer
    int isSelect = retrouve_premier_buffer(buffer);

    // On deplace notre curseur dans le buffer
    deplacement_curseur_dans_buffer(c);
    // Et on deplace dans l'affiche graphique
    print_position_curseur(getCurLineBuffer(buffer),getCurColBuffer(buffer));

    // Si l'affiche scroll, ie deplacer  renvoie un nombre != 0, ou si il y avait une selection
    if(deplacer(getCurLineScr(buffer,getCurLineBuffer(buffer)),getCurColScr(buffer))>0 || isSelect != 0 ){
        // Alors on affiche le buffer
        affiche_buffer();
    }
    return 1;
}


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


int traitement_char(char c){
    // On analyse pour savoir si il s'agit d'un charactere commun ou \n \t ...
    switch (c) {
        case '\t':
            return -1;
        default:{
            // Pour la robustesse on va allouer une variable avant de la donner
            // au buffer, puisque le buffer utilise strlen il a besoin d'un \0
            char * tmp = calloc(2,sizeof(char));
            tmp[0]=c;
            // Puis ajouter le tableau cree au buffers
            addTextBuffer(buffer,tmp);
            free(tmp);
            break;
        }
    }
    // Enfin on va s'occuper d'afficher le nouveau buffer
    affiche_buffer();
    return 1;
}



/**
 * Retourne une constante de direction selon le caractère saisi
 * Qui est formaté pour le buffer
 */
int direction(char c){
    switch(c){
        case 'u':
            return BUFFER_CUR_UP;
        case 'd':
            return BUFFER_CUR_DOWN;
        case 'r':
            return BUFFER_CUR_RIGHT;
        case 'l':
            return BUFFER_CUR_LEFT;
        default:
            return -1;
    }
}

/**
 * Charger un fichier à ppartir d'un buffer déjà existant
 * Cette fonction enregistre d'abord le contenu du buffer
 */

int charger(){
    sauvegarde_editeur();
    demande_nom_fichier_chargement();

    char * buf_fichier;
    if(chargement(&buf_fichier,nom_du_fichier)==(-1)) {
        nom_du_fichier= NULL;
        return -1;
    }
    //on libère le buffer précédent pour un nouveau
    freeBuffer(buffer);
    buffer = newBuffer(10);
    // Si le fichier existe on l'ajoute a notre buffer
    addTextBuffer(buffer,buf_fichier);
    moveCursorPositionBuffer(buffer,0,0);
    deplacer(0,0);
    initialisation_undoredo();
    affiche_buffer();
    //free(buf_fichier);
    return 1;
}



/**
    Affiche le buffer, puis pour chaque partie qui fait partie de la selection de
    find le print en rouge

    return 1 si tous s'est bien passé
*/

int affiche_buffer_find(Position * p, char * toFind){
    affiche_buffer(); // on affiche le buffer
    int premier_line=get_premier_line();
    for(int i = 0 ; i < getNbPosition(p) ; ++i){ // pour chaque positon
        int* pos = getPosition(p, i);
        if(pos[0] >= premier_line && pos[0]-premier_line<get_LINES()-1){ // si elle est dans l'ecran
            char * ligne_entier = getLineBuffer(buffer,getCurLineScr(buffer,pos[0]));
            char * ligne_partiel = strndup(ligne_entier+pos[1],strlen(toFind));
            color_select_zone(getCurLineScr(buffer,pos[0]),pos[1],ligne_partiel,2); // on l'affiche
        }
    }
    char * mode_find = malloc(40); // On continue en signalent a l'user tous les match du find
    sprintf(mode_find," : MODE FIND   %d  %s trouvé",getNbPosition(p),toFind);
    // On affiche proprement notre resultat a la fin
    print_info(mode_find,line_wrapping,getCurLineBuffer(buffer),getCurColBuffer(buffer));
    deplacer(getCurLineScr(buffer,getCurLineBuffer(buffer)),getCurColScr(buffer));
    refresh();
    free(mode_find);
    return 1;
}


/**
    Recuperer grace a findStringBuffer du module buffer les positions des elements
    correspondant a toFind et affiche sur la console

    return 1 si tous c'est bien passé
*/
int find_select(char * toFind){
    Position * position = findStringBuffer(buffer,toFind);
    affiche_buffer_find(position, toFind);
    int again = 1;
    //C'est ici qu'on relance en quelque sort la boucle principale, c'est la boucle
    //sur le getentry qui fait qu" l'on effectuer tous les action normal !
    //C'est aussi ca qui permet avec ESC ou ctrl+q de revenir en arriere sur les find
    while (again>0) { // c'est quasi la meme boucle que boucle_utilisateur
        struct command * entrer_user = malloc(sizeof(struct command));
        int entrer=getentry(entrer_user);
        if(entrer==-1){
            // on affiche proprement que la recupereration de l'user a echoué
            print_info("La recupperation du charactere a echoué, requette.c getentry"
                        ,line_wrapping,getCurLineBuffer(buffer),getCurColBuffer(buffer));
            deplacer(getCurLineScr(buffer,getCurLineBuffer(buffer)),getCurColScr(buffer));
        }
        again=traitement_entrer(entrer,entrer_user);
        // c'est ici que l'on difere de boucle_utilisateur, car on a un find a afficher
        position = findStringBuffer(buffer,toFind);
        affiche_buffer_find(position, toFind);
        free(entrer_user);
    }


    free(position);
    affiche_buffer();
    refresh();
    return 1;
}



/**
    Fonction qui est appeler si la commande find est active sans zone de selection
    qui va d'abord demander a l'utilisateur de rentrer des char a finder et
    ensuite le laisser chercher dans son document avec find_select(char * toFind)
    La chaine toFind sera ce qu'il a remplie
*/
int find(){
    char toFind[50]; // initialisation
    toFind[0]='\0';
    Position * position;
    int again = 0; // va nous servir pour l'index dans le table,
    while (again>=0) { // mais aussi pour la boucle
        char * mode_find = malloc(50);
        struct command * entrer_user = malloc(sizeof(struct command));
        int entrer=getentry(entrer_user);
        if(entrer==-1){
            print_info("La recupperation du charactere a echoué, requette.c getentry"
                        ,line_wrapping,getCurLineBuffer(buffer),getCurColBuffer(buffer));
            deplacer(getCurLineScr(buffer,getCurLineBuffer(buffer)),getCurColScr(buffer));
        }
        switch (entrer) {
            case QUIT: // Cas ou l'utilisateur veux terminer sa session
                again=-1;
                break;
            case INSERT:
                toFind[again]=entrer_user->c;
                again++;
                toFind[again]='\0';
                break;
            case DELETEBACK:
                if(again > 0){
                    again--;
                    toFind[again]='\0';
                }
                break;
            default:
                break;
        } // fin switch
        // on fait Attention a ce qu'il y est au moins un char de taper
        if (again>0) {
            position = findStringBuffer(buffer,toFind);
            // On affiche le buffer
            affiche_buffer_find(position, toFind);
        }
        deplacer(getCurLineScr(buffer,getCurLineBuffer(buffer)),getCurColScr(buffer));
        // On libere la structure quand sont traiement est effectuer
        free(entrer_user);
        free(mode_find);
        refresh();
    }
    free(position);
    find_select(toFind);
    affiche_buffer();
    refresh();
    return 1;
}


/**
 * Demander le num du raccourci à modifier (chaque raccourci)
 */
char * demande_num_raccourci(){
    char * nom_raccourci = calloc(200,sizeof(char));
    strcpy(nom_raccourci,"\n\nQuel raccourci vous voulez modifier?\nCUT 1 / COPY 2 / PASTE 3 / SAVE 4 / DELETE 5 / QUIT 6\nUNDO 7 / REDO 8 / FIND 9 / OPEN 10 / FORMAT 11 / MODIFY 12 \nVeuillez saisir le numéro du raccourci\n");
    affichage(nom_raccourci,40,6,0,0,"");
    free(nom_raccourci);
    return demande_nom_fichier();
}


/**
 *  Tester si un raccourci est déjà utilisé dans le fichier de configuration
 *  Retour : 1 si déjà utilisé, 0 sinon
 */

int dejaUtilise(char c){
    FILE * config = fopen("configuration.config","r");
    if(config==NULL) return -1;
    char* s = malloc (sizeof(char)*256);
    while(fgets(s,256,config)!=NULL){
        char * token = strtok(s," ");
        token = strtok(NULL," ");
        if(c==token[1]){
            free(s);
            fclose(config);
            return 1;
        }
    }
    free(s);
    fclose(config);
    return 0;
}
/**
 * Demande le nouveau raccourci, vérifie qu'il s'agit d'une lettre
 * et vérifie qu'il n'est pass déjà exploité
 */
char demande_raccourci(){
    char * nom_raccourci = calloc(200,sizeof(char));
    strcpy(nom_raccourci,"\n\nQuel est votre nouveau raccourci ?\nVeuillez choisir une lettre, Votre raccourci sera Ctrl+cette_lettre\n");
    affichage(nom_raccourci,40,6,0,0,"");
    char* c;
    do {
        c = demande_nom_fichier();
        if(c!=NULL)
            if(isalpha(c[0]) && !dejaUtilise(toupper(c[0])))
                return toupper(c[0]);
            c = NULL;
    }while(c==NULL) ;
    return ' ';
}

/**
 * Cette fonction est une fonction auxiliaire 
 * qui permet de faire la correspondance entre les
 * numéros de lignes dans le fichier de configuration 
 * et les raccourcis auquels elles correspondent 
 */ 
char * correspondance(int ligne){
    switch(ligne){
        case 1 :
            return "CUT:";
        case 2:
            return "COPY:";
        case 3:
            return "PASTE:";
        case 4:
            return "SAVE:";
        case 5:
            return "DELETE:";
        case 6:
            return "QUIT:";
        case 7:
            return "UNDO:";
        case 8:
            return "REDO:";
        case 9:
            return "FIND:";
        case 10:
            return "OPEN:";
        case 11:
            return "FORMAT:";
        case 12:
            return "MODIFY:";
        case 13:
            return "LINE_WRAPPING:";
        default:
            return NULL;
    }
}

 /**
  * Modification des raccourcis
  */
int modifier(){
    FILE * config_file = fopen("configuration.config","r");
    FILE * tmp = fopen("tmp.txt","w");
    if(config_file==NULL) return -1;
    int ligne = atoi(demande_num_raccourci());
    if(ligne==0) return -1;
    int i=0;
    char *s = malloc(sizeof(char)*256) ;
    char c = demande_raccourci();
    affiche_buffer();
    while(fgets(s,256,config_file)!=NULL){
        i++;
        if(i==ligne && correspondance(ligne)!=NULL){
            fputs(correspondance(ligne),tmp);
            fputs(" ^",tmp);
            fputc(c,tmp);
            fputc('\n',tmp);

        }else{
            fputs(s,tmp);
        }
    }
    fclose(config_file);
    fclose(tmp);
    int ret = remove("configuration.config");
    if(ret==0){
        rename("tmp.txt", "configuration.config");
    }
    free(s);
    remove("tmp.txt");
    read_configuration_file();
    return 1;
}
/**
 * Crée le menu en faisant appel à affichage 
 * Permet de choisir une option parmi les fonctionnalités du menu  
 * retour : 
 *      1 au succès 
 *      0 quand c'est quit
 *      -1 en cas d'erreur ou dasn le cas de cut 
 */
int choisir(){
    int num = create_menu();

    switch (num){
        case 0:
            return -1;
        case 1: //Copy
            nb_de_modification_sans_sauvagarde+=10;
            copyBuffer(buffer);
            affiche_buffer();
            return 1;
        case 2: //Paste
            coller(buffer);
            affiche_buffer();
            return 1;
        case 3: //save
            return sauvegarde_editeur();
        case 4: //delete
            supprimer(buffer);
            affiche_buffer();
            return 1;
        case 5: //quit
            return 0;
        case 6: //undo
            nb_de_modification_sans_sauvagarde+=10;
            undoRedo(buffer,-1);
            affiche_buffer();
            return 1;
        case 7://redo
            nb_de_modification_sans_sauvagarde+=10;
            undoRedo(buffer,1);
            affiche_buffer();
            return 1;
        case 8: //find
            // Si on a une zone de selection, alors on va la chercher dans le buffer
            if (retrouve_premier_buffer(buffer)!=0) {
                return find_select(getSelectionZoneBuffer(buffer)); // return 1
            }
            else{
                return find(); // return 1
            }
        case 9: //open
            return charger(); // return 1
        case 10: //format
            formatColumnWidthBuffer(buffer);
            affiche_buffer();
            return 1;
        case 11: //modify
            modifier();
            return 1;
        case 12:
            line_wrapping=(line_wrapping+1)%2;
            affiche_buffer();
            return 1;
        default:
            return -1;
    }
}

    /**
        Fonction qui traite les entrès de l'utilisateur, en fonction de leur type
            et qui dans chaque cas invoque la bonne methode pour le programme

        int entrer : le type de l'entrer QUIT, INSERT, COPY, CUT ...

        struct command * entrer_user :  la structure qui contient le charactere
                                        entrer par l'user

        return -1 en cas d'echec, 0 si QUIT et 1 sinon
    */

int traitement_entrer(int entrer, struct command * entrer_user){
    switch (entrer)
    {
    case INSERT:
        inserer(buffer,entrer_user->c);
        affiche_buffer();
        return 1;
    case MOVE:
        return deplacement_curseur(&entrer_user->c); // return 1
    case SELECT:
        moveCursorSelectBuffer(buffer, direction(entrer_user->c),1);
        affiche_buffer();
        return 1;
    case COPY:
        nb_de_modification_sans_sauvagarde+=10;
        copyBuffer(buffer);
        affiche_buffer();
        return 1;
    case PASTE:
        coller(buffer);
        affiche_buffer();
        return 1;
    case DELETE:
        supprimer(buffer);
        affiche_buffer();
        return 1;
    case DELETEBACK:
        suprimerBack(buffer);
        affiche_buffer();
        return 1;
    case UNDO:
        nb_de_modification_sans_sauvagarde+=10;
        undoRedo(buffer,-1);
        affiche_buffer();
        return 1;
    case REDO:
        nb_de_modification_sans_sauvagarde+=10;
        undoRedo(buffer,1);
        affiche_buffer();
        return 1;
    case OPEN:
        return charger(); // return 1
    case SAVE:
        return sauvegarde_editeur(); // return 1
    case QUIT:
        return 0;
    case REDIMENTION:
        return redimensionnement(); // return 1
    case FIND:
        // Si on a une zone de selection, alors on va la chercher dans le buffer
        if (retrouve_premier_buffer(buffer)!=0) {
            return find_select(getSelectionZoneBuffer(buffer)); // return 1
        }
        else{
            return find(); // return 1
        }
    case FORMAT:
        formatColumnWidthBuffer(buffer);
        affiche_buffer();
        return 1;
    case MODIFY:
        modifier();
        return 1;
    case MENU:
        return choisir();
    case LINE_WRAPPING:
        line_wrapping=(line_wrapping+1)%2;
        affiche_buffer();
        return 1;
    default:
        return -1;
    }
}

void boucle_utilisateur(){
  //sera gere  par un autre thread, renplacer la boucle infini
    int again = 1;
    nb_de_modification_sans_sauvagarde = 0;
    while (again) {

        // Si on a une valeur superieur a 100 on sauvegarde automatiquement le ficher
        if (nb_de_modification_sans_sauvagarde > 100 && nom_du_fichier!=NULL) {
            sauvegarde_editeur();
            nb_de_modification_sans_sauvagarde = 0;
        }

        // On recuppere l'entrer de l'utilisateur, c'est a dire que getEntrer va nous
        // remplir le caractere entrer dans char_entrer_user, et nous retourner
        // le type dans un variable type_entrer_user
        struct command * entrer_user = malloc(sizeof(struct command));
        int entrer=getentry(entrer_user);
        if(entrer==-1){
            print_info("La recupperation du charactere a echoué, requette.c getentry"
                        ,line_wrapping,getCurLineBuffer(buffer),getCurColBuffer(buffer));
            deplacer(getCurLineScr(buffer,getCurLineBuffer(buffer)),getCurColScr(buffer));
        }
        switch (traitement_entrer(entrer,entrer_user)) {
            case -1:
                // Cas d'une erreur dans le traiement d'un charactere
                print_info("Le traiement du caractere a échouer, traitement_char"
                            ,line_wrapping,getCurLineBuffer(buffer),getCurColBuffer(buffer));
                deplacer(getCurLineScr(buffer,getCurLineBuffer(buffer)),getCurColScr(buffer));
                break;
            case 0: // Cas ou l'utilisateur veux terminer sa session
                again=sortie();
                break;
            case 1:

                break;
            default:
                break;
        } // fin switch

        // On libere la structure quand sont traiement est effectuer
        free(entrer_user);

    } // fin while
    freeBuffer(buffer);
    quitter_affichage();
}

int initialisation(int flag,char * nomDeFichier){
    buffer = newBuffer(10);
    initialisation_undoredo();


    //si le flag est a 1 on charge le fichier
    if (flag==1){
        char * buf_fichier;
        //Impossible de charger le fichier donner en argument
        nom_du_fichier= nomDeFichier;
        if(chargement(&buf_fichier,nomDeFichier)==(-1)){
            print_info("Impossible de charger le fichier",line_wrapping,getCurLineBuffer(buffer)
                    ,getCurColBuffer(buffer));
            deplacer(getCurLineScr(buffer,getCurLineBuffer(buffer)),getCurColScr(buffer));
            free(buf_fichier);
            exit(0);
        }
        // Si le fichier existe on l'ajoute a notre buffer
        addTextBuffer(buffer,buf_fichier);
        //free(buf_fichier);
    }

    //on lance la fenetre d'affichage
    if(initialisation_fenetre()==-1){
        printf("Impossible d'initialiser la fenetre graphique\n");
        sortie_erreur();
    }
    if(initialisation_requete()==-1){
        printf("Impossible d'initialiser les requette utilisateur\n");
        sortie_erreur();
    }
    moveCursorPositionBuffer(buffer,0,0);
    affiche_buffer();
    deplacer(0,0);
    //on lance la boucle pour les entrer utilisateur
    boucle_utilisateur();

    return 0;
}

int main(int argc, char *argv[]) {
    // Si on a un argument, i.e. un fichier a charger
    if (argc==2) {
        initialisation(1,argv[1]);
    }
    // Sinon on a pas d'argument le fichier n'est pas ouvert, le flag est mis a 0
    else{
        initialisation(0,NULL);
    }
    return 0;
}
