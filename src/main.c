
#include <stdio.h>
#include <stdlib.h>
#include "mfile.h"

#define LEN 512

int main(void)
{
	Init();
	printf("------Creation : -------\n");
	mfifo * fifo = mfifo_connect("test",O_CREAT,0777,LEN);

	printf("\n\n-----Connexion : ----\n");
	fifo = mfifo_connect("test",0,0777,LEN);



	printf("\n\n-------------------------------------------\n");

	printf("Debut du pointeur fifo : %ld \n", fifo->debut );
	printf("Cap. fifo : %ld \n" , fifo->capacity);
	printf("Fin du pointeur fifo : %ld \n", fifo->fin );

	printf("\n\n-----------Lecture depuis le main :-----------\n> ");

	for ( int i = 0 ; i < fifo->capacity ; i++ ){
		if ( fifo->memory[i] > '!' && fifo->memory[i] < '~')
			printf("%c", fifo->memory[i] );
	}


	printf("\n\n-------------------------------------------\n");

	printf("\n\nCreation - connexion : \n");
	mfifo * b = mfifo_connect("testBis",O_CREAT|O_EXCL,0777,LEN);

	


	return EXIT_SUCCESS;
}
					
