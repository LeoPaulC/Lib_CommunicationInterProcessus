
#include <stdio.h>
#include <stdlib.h>
#include "mfile.h"

#define LEN 512

int main(void)
{
	Init();
	printf("------ Creation PUIS Connexion : -------\n");
	mfifo * fifo = mfifo_connect("mfifoCo",O_CREAT,0777,LEN);
	fifo = mfifo_connect("mfifoCo",0,0777,LEN);

	printf("Debut du pointeur fifo : %ld \n", fifo->debut );
	printf("Cap. fifo : %ld \n" , fifo->capacity);
	printf("Fin du pointeur fifo : %ld \n", fifo->fin );
	printf("Pid du fifo : %d \n", fifo->pid );

	printf("\n------ Creation ET connexion : -------\n");
	mfifo * b = mfifo_connect("testBis",O_CREAT|O_EXCL,0777,LEN);

	printf("Debut du pointeur fifo : %ld \n", b->debut );
	printf("Cap. fifo : %ld \n" , b->capacity);
	printf("Fin du pointeur fifo : %ld \n", b->fin );
	printf("Pid du fifo : %d \n", b->pid );

	printf("On tente une ecriture dans mfifo : \n");
	write_addr("abcdefgh",b);

	printf("\n\n------ Creation Tube anonyme : -------\n");
	mfifo * c = mfifo_connect(NULL,	O_CREAT|O_EXCL,0777,100);
	printf("Debut du pointeur fifo : %ld \n", c->debut );
	printf("Cap. fifo : %ld \n" , c->capacity);
	printf("Fin du pointeur fifo : %ld \n", c->fin );
	printf("Pid du fifo : %d \n", c->pid );


	return EXIT_SUCCESS;
}
					
