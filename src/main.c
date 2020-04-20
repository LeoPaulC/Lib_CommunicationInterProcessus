
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
	printf("capacity : %ld \n",mfifo_capacity(fifo) );
	printf("free memory : %ld \n",mfifo_free_memory(fifo) );

	printf("\n------ Creation ET connexion : -------\n");
	mfifo * b = mfifo_connect("testBis",O_CREAT|O_EXCL,0777,LEN);


	char* buf = "Ceci est un test d'Ecriture !" ;
	int res_write = mfifo_write(fifo,buf,strlen(buf));

	char* bu = "Coucou test" ;
	res_write = mfifo_write(fifo,bu,strlen(bu));

	sem_getvalue(&fifo->sem, &val);
	char * buf_read = malloc(sizeof(char)*strlen(buf)+1) ;

	size_t res_read = mfifo_read(fifo, buf_read,strlen(buf));

	mfifo_disconnect(fifo);
	mfifo_unlink(fifo->nom);

	printf("taille suppr : %d\n",free_mfifo(fifo));
	printf("Etat apres suppression\n" );
	printf("Contenu du dossier /dev/shm/ : \n" );
	execlp("ls","ls","/dev/shm/",NULL);
	


	return EXIT_SUCCESS;
}
					
