
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mfile.h"

#define LEN 1024

int main(void)
{
	Init();
	printf("------ Creation PUIS Connexion : -------\n");

	mfifo * fifo = mfifo_connect("testBis",O_CREAT,0777,LEN);

	//char* buf = "Ceci est un test d'Ecriture !" ;
	//int res_write = mfifo_write(fifo,buf,strlen(buf));
	//int val;
	//sem_getvalue(&fifo->sem, &val);
	//printf("capacity : %ld \n",mfifo_capacity(fifo) );
	//printf("free memory : %ld \n",mfifo_free_memory(fifo) );

	//printf("\n------ Creation ET connexion : -------\n");
	fifo = mfifo_connect("testBis",0,0777,LEN);

	char* buf = "Ceci est un test d'Ecriture !" ;
	int res_write = mfifo_write(fifo,buf,strlen(buf));
	printf("b->memory : %s\n", fifo->memory );

	mfifo * c  = mfifo_connect("testBis",0,0777,LEN);
	printf("c->memory : %s\n", c->memory );

	printf("\nLECTURE.....\n");
	char * b = malloc(50);
	int res_read = mfifo_read(fifo, b, 10);


	mfifo_disconnect(fifo);
	mfifo_unlink(fifo->nom);
	//printf("Etat apres suppression\n" );
	printf("Contenu du dossier /dev/shm/ : \n" );
	//execlp("ls","ls","/dev/shm/",NULL);
	


	return EXIT_SUCCESS;
}
					
