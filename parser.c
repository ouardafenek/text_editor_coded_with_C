#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"



/**
 * Un stucture qui permettera de bien décomposer un message et de sauvegarder 
 * toutes les informatiosn qu'il contient 
 */
struct split* newSplit(){
    struct split* retour = malloc(sizeof(struct split));

    retour->nb_message = 0;
    retour->taille_max = 10;
    retour->messages = malloc(retour->taille_max*sizeof(char*));

    return retour;
}

/**
 * selon le nombre d'informations contenues dans le message la memoire du split est allouée
 */
void add_split(struct split* split_message, char* message){
    if(split_message->nb_message == split_message->taille_max){
        split_message->taille_max *= 2;
        split_message->messages = realloc(split_message->messages, split_message->taille_max*sizeof(char*));
        
	if(split_message->messages == NULL){
            perror("Echec de l'allocation de la mémoire");
            exit(EXIT_FAILURE);
        }
    }

    split_message->messages[split_message->nb_message++] = message;
}

/**
 * libérer la memoire 
 */ 
void free_split(struct split* split_message){
    free(split_message->messages);
    free(split_message);
}

/**
 * Découpe le message en petit bouts pour un traitement simplifié
 *
 * message: le message a parser
 *
 * retour une structure Split où se trouve les informations
 */
struct split* message_parser(char* message){
    const char separateur[2] = "!>";

    struct split* split_messages = newSplit();

    char* token;

    token = strtok(message, separateur);


    do{ // On ajoute les differentes parties du message
        add_split(split_messages, token);
        token = strtok(NULL, separateur);
    }while(token != NULL);

    char* dernier_message = split_messages->messages[split_messages->nb_message-1];
    dernier_message[strlen(dernier_message)] = '\0';
    return split_messages;

}
