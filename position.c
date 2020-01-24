#include <stdio.h>
#include <stdlib.h>

#include "position.h"

struct position{
	int nb_position;
	int nb_position_max;
	int** positions;
};

struct position* newPosition(){
	struct position* position = malloc(sizeof(struct position));	
	
	position->nb_position = 0;
	position->nb_position_max = 10;
	position->positions = malloc(position->nb_position_max * sizeof(int*));
	
	return position;
}

int getNbPosition(struct position* position){
	return position->nb_position;
}

int addPosition(struct position* position , int x, int y){

	// S'il n'y a plus assez de place
	if(position->nb_position >= position->nb_position_max){
		int** tmp = realloc(position->positions, sizeof(int*) * position->nb_position_max * 2);
		
		if(tmp == NULL){
			perror("Erreur d'allocation mémoire : position#addPosition");
			return -1;
		}
		
		position->nb_position_max *= 2;
		position->positions = tmp;
	 }
 
	// Alloue la mémoire pour deux entiers
	int* a_ajouter = malloc(2 * sizeof(int));
	a_ajouter[0] = x;
	a_ajouter[1] = y;
	
	position->positions[position->nb_position++] = a_ajouter;
	
	return 0;	
}

int* getPosition(struct position* position, int index){
	return position->positions[index];
}

void freePosition(struct position* position){
	for(int i = 0 ; i < position->nb_position ; ++i){
		free(getPosition(position, i));
	}
	free(position->positions);
	free(position);
}
