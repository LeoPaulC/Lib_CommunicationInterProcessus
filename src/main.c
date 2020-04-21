
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
		printf("\n\n----------Fils -------------- \n");
		char* buf = "Ceci est un test d'Ecriture !" ;
		mfifo * fifo_enfant = mfifo_connect("testBis",0,0777,LEN);
		int res_write = mfifo_write(fifo_enfant,buf,strlen(buf));

		printf("Fifo_enfant Content : \n> " );		
		printf("%s",  &fifo_enfant->memory );

		const char * buff = "testEcritureDepuisUnWrite" ;
		int res = mfifo_write(fifo_enfant, buff, strlen(buff));
		//int res = write(fd,"testEcritureDepuisUnWrite",strlen("testEcritureDepuisUnWrite"));
    	printf("Res du Write dans Fils : len %d , content : %s \n", res , buff );

		/*
		int r = msync(&fifo_enfant->memory, sysconf(_SC_PAGESIZE) , MS_SYNC);
		if ( r == -1 ) perror("Msync:");
		printf("\nRetour de Msync : %d \n", r );
		*/
		return EXIT_SUCCESS ;
	}
	else {
		//sleep(2);
		waitpid(pid , &status , 0);
		printf("\n\n------------ Pere ---------------\n");
		mfifo * c  = mfifo_connect("testBis",0,0777,LEN);

		printf("c->memory : %s\n", &c->memory );

		char * buf = malloc(sizeof(char)* getpagesize() );
    	
    	int res = mfifo_read(c, buf, 10 ) ; // 10 pour nos test
		//int res = read(fd,buf,getpagesize()) ;
    	printf("Res du READ de fd : %s\n\n", buf );
		mfifo_disconnect(c);
		mfifo_unlink(c->nom);
		
		//printf("Etat apres suppression\n" );
		printf("Contenu du dossier /dev/shm/ : \n" );
		//execlp("ls","ls","/dev/shm/",NULL);

	}


	

	printf("\n\n");

	mfifo_disconnect(fifo);
	mfifo_unlink(fifo->nom);

	
	


	return EXIT_SUCCESS;
}
					
