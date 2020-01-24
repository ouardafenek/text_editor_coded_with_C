#ifndef POSITION_H
#define POSITION_H

typedef struct position Position;

/**
 * Alloue la mémoire pour une structure position
 *
 * retour : L'adresse de la position
 */
Position* newPosition();

/**
 * Recupère le nombre de positions disponible dans une structure position
 *
 * position : la structure position dans laquelle rechercher
 *
 * Retour: le nombre de positions disponible
 */
int getNbPosition(Position*);

/**
 * Ajoute une nouvelle position dans la structure position
 *
 * position : la structure position dans laquelle ajouter
 *
 * x: la première coordonnée
 *
 * y: la deuxième coordonnée
 *
 * Retour: 0 si l'ajout est effectuée, -1 en cas d'erreur
 */
int addPosition(Position*, int, int);

/**
 * Récupère une position donnée dans une structure position
 *
 * position: la structure dans lequel chercher
 * 
 * ind: le no de la position à récupérer
 *
 * Retour un tableau de deux entiers correspondant à x et à y
 */
int* getPosition(Position*, int);

/**
 * Libère les ressources allouées à une structure position
 * 
 * position : la structure position
 */
void freePosition(Position*);


#endif 
