#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "buffer.h"

void printBuffer(struct Buffer* buffer){
	char* result = getTextBuffer(buffer);

	printf("|%s|\n", result);

	free(result);
}

int main(int argc, char** argv){

	Buffer* buffer = newBuffer(10);

	
	addTextBuffer(buffer, "aaa\nbbb");
	printBuffer(buffer);
	
	moveCursorPositionBuffer(buffer, 0, 0);
	
	for(int i = 0 ; i < 9 ; ++i){
		char* selection = getSelectionZoneBuffer(buffer);
		printf("Selection : |%s|\n", selection);
		free(selection);
		printf("Position cur sel col : %d\n\n", getCurSelectColBuffer(buffer));
		
		moveCursorSelectBuffer(buffer, BUFFER_CUR_RIGHT, 1);
	}
	
	addTextBuffer(buffer, "Efface");
	

	printBuffer(buffer);


	freeBuffer(buffer);

	exit(EXIT_SUCCESS);
}
