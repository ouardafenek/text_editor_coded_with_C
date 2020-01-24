#ifndef PARSER_H
#define PARSER_H


struct split{
    char** messages;
    int nb_message;
    int taille_max;

}; 

/**
 * Un stucture qui permettera de bien décomposer un message et de sauvegarder 
 * toutes les informatiosn qu'il contient 
 */
struct split* newSplit();
/**
 * selon le nombre d'informations contenues dans le message la memoire du split est allouée
 */
void add_split(struct split* split_message, char* message);

/**
 * libérer la memoire 
 */ 
void free_split(struct split* split_message);
/**
 * Découpe le message en petit bouts pour un traitement simplifié
 *
 * message: le message a parser
 *
 * retour une structure Split où se trouve les informations
 */
struct split* message_parser(char* message);


#endif