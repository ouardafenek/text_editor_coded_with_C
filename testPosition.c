#include <stdio.h>
#include <stdlib.h>

#include "position.h"

int main(){

	Position* position = newPosition();
	printf("Création réussi");
	
	for(int i = 0 ; i < 15 ; ++i){
		printf("Ajout de %d\n", i);
		addPosition(position, i, i+i);
	}
	
	printf("Il y a %d positions\n", getNbPosition(position));
	
	for(int i = 0 ; i < getNbPosition(position) ; ++i){
		int* pos = getPosition(position, i);
		
		printf("%d - %d\n", pos[0], pos[1]);
		
	}
	
	freePosition(position);

}
