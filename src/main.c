
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
	int val;
	sem_getvalue(&fifo->sem, &val);
	printf("capacity : %ld \n",mfifo_capacity(fifo) );
	printf("free memory : %ld \n",mfifo_free_memory(fifo) );

	printf("\n------ Creation ET connexion : -------\n");
	mfifo * b = mfifo_connect("testBis",O_CREAT|O_EXCL,0777,LEN);


	char* buf = "Ceci est un test d'Ecriture !" ;
	int res_write = mfifo_write(fifo,buf,strlen(buf));

	char* bu = "Coucou test" ;
	res_write = mfifo_write(fifo,bu,strlen(bu));

	printf("etat memory : %s\n", fifo->memory );

	fifo = mfifo_connect("testBis",0,0777,LEN);
	mfifo_disconnect(fifo);
	mfifo_unlink(fifo->nom);
	//printf("Etat apres suppression\n" );
	printf("Contenu du dossier /dev/shm/ : \n" );
	execlp("ls","ls","/dev/shm/",NULL);
	


	return EXIT_SUCCESS;
}
					
