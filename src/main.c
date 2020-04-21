
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
	int status = 0 ;
	pid_t pid = fork();

	if ( pid == 0 ) {
		printf("\n----------Fils :: \n");
		char* buf = "Ceci est un test d'Ecriture !" ;
		mfifo * fifo_enfant = mfifo_connect("testBis",0,0777,LEN);
		int res_write = mfifo_write(fifo_enfant,buf,strlen(buf));
		printf("b->memory : %s\n", fifo_enfant->memory );
		//printf("LEcture FIls :\n");
		printf("------------------Fin Fils , ecriture reussis ---------\n");
		return 0 ;
	}
	else {
		//sleep(2);
		waitpid(pid , &status , 0);
		printf("\n------------Pere :\n");
		mfifo * c  = mfifo_connect("testBis",0,0777,LEN);
		printf("c->memory : %p\n", c->memory );
		printf("\nLECTURE PERE .....\n");
		char * b = malloc(10);
		int res_read = mfifo_read(c, b, 10);

		printf("\n Test creation de mfifo dans le pere : \n");
		mfifo * fifo_papa = mfifo_connect("testPere",O_CREAT,0777,LEN);
		printf("\nFin Creation\n");

		mfifo_disconnect(fifo_papa);
		mfifo_unlink(fifo_papa->nom);

	}


	


	mfifo_disconnect(fifo);
	mfifo_unlink(fifo->nom);
	//printf("Etat apres suppression\n" );
	printf("Contenu du dossier /dev/shm/ : \n" );
	//execlp("ls","ls","/dev/shm/",NULL);
	


	return EXIT_SUCCESS;
}
					
