
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mfile.h"

#define LEN 512

int main(void)
{
	Init();
	printf("------ Creation PUIS Connexion : -------\n");
	mfifo * fifo = mfifo_connect("testBis",O_CREAT,0777,LEN);
	fifo = mfifo_connect("testBis",0,0777,LEN);
	int val;
	sem_getvalue(&fifo->sem, &val);

	printf("valeur semaphore main: %d \n",val );

	printf("\n------ Creation ET connexion : -------\n");
	mfifo * b = mfifo_connect("testBis",O_CREAT|O_EXCL,0777,LEN);

	printf("Debut du pointeur fifo : %ld \n", b->debut );
	printf("Cap. fifo : %ld \n" , b->capacity);
	printf("Fin du pointeur fifo : %ld \n", b->fin );
	printf("Pid du fifo : %d \n", b->pid );

	printf("On tente une ecriture dans mfifo : \n");

	sem_getvalue(&fifo->sem, &val);
	printf("valeur semaphore main: %d \n",val );
	
	char* buf = "abcd" ;
	int res_write = mfifo_write(fifo,buf,strlen(buf));
	sem_getvalue(&fifo->sem, &val);
	printf("valeur semaphore main: %d \n",val );
	printf("Res write : %d\n", res_write );	

	char * buf_read = malloc(sizeof(char)*strlen(buf)) ;
	size_t res_read = mfifo_read(fifo, buf_read, sizeof(buf_read));
	printf("\nRes read : %d\n " , res_read);

	/*
	printf("\n\n------ Creation Tube anonyme : -------\n");
	mfifo * c = mfifo_connect(NULL,	O_CREAT|O_EXCL,0777,100);
	printf("Debut du pointeur fifo : %ld \n", c->debut );
	printf("Cap. fifo : %ld \n" , c->capacity);
	printf("Fin du pointeur fifo : %ld \n", c->fin );
	printf("Pid du fifo : %d \n", c->pid );
	*/

	printf("---- Test de suppression de mfifo : %s ------ \n\n", fifo->nom);


	printf("Etat avant suppression\n" );
	//printf("Contenu du dossier /dev/shm/ : \n" );
	//execlp("ls","ls","/dev/shm/",NULL);


	destroy(fifo);
	printf("deconnexion de : %s\n",fifo->nom );
	mfifo_disconnect(fifo);

	printf("suppression de : %s\n",fifo->nom );
	mfifo_unlink(fifo->nom);

	printf("Etat apres suppression\n" );
	printf("Contenu du dossier /dev/shm/ : \n" );
	execlp("ls","ls","/dev/shm/",NULL);


	return EXIT_SUCCESS;
}
					
