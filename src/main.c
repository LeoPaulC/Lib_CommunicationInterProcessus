
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mfile.h"

#define LEN 1024

int main(void)
{
	Init();
	printf("------ Creation PUIS Connexion : -------\n");
	
	mfifo * fifo = mfifo_connect("testBis",O_CREAT|O_EXCL,0777,LEN);

	printf("capa %d  \n", fifo->capacity );
	printf("fifo->memory :  %s\n", fifo->memory );

	char* buf = "Nous avons enfin un test concluant Chef :)" ;
<<<<<<< HEAD
	//message * res = malloc(sizeof(message));

	//create_message(buf,res);

	int r = mfifo_write(fifo,buf,strlen(buf));

	printf("Main l du message dans fifo->memory :  %s\n", &fifo->memory[0] );
	printf("Main content fifo->memory :  %s\n", &fifo->memory[7] );
	/*printf("taille message %ld\n",sizeof(res)+res->l+1 );
	printf("affchage message :\n");
	printf("longueur :%ld\n", res->l );
	printf("contenu :%s\n", res->mes );
	printf("ecriture du message\n" );		*/
	/*
=======
	int r = mfifo_write(fifo,buf,(strlen(buf)));
	//printf("fifo cap main %s\n",fifo->memory );
	
>>>>>>> lecture
	int status = 0 ;
	

	printf("main cap %ld\n", fifo->capacity );
	printf("main memory %s\n", fifo->memory );

	pid_t pid = fork();

	if ( pid == 0 ) {

		printf("\n\n----------Fils -------------- \n");
		mfifo * fifo_enfant = mfifo_connect("testBis",0,0777,LEN);
		

		/*const char * buff = "20 testEcritureDepuisUnWrite" ;
		int res = mfifo_write(fifo_enfant, buff, strlen(buff));
		//int res = write(fd,"testEcritureDepuisUnWrite",strlen("testEcritureDepuisUnWrite"));
    	printf("Res du Write dans Fils : len %d , content : %s \n", res , buff );

		/*
		int r = msync(&fifo_enfant->memory, sysconf(_SC_PAGESIZE) , MS_SYNC);
		if ( r == -1 ) perror("Msync:");
		printf("\nRetour de Msync : %d \n", r );
		*/
	/*
		return EXIT_SUCCESS ;
	}
	else {
		//sleep(2);
		waitpid(pid , &status , 0);
		printf("\n\n------------ Pere ---------------\n");
		mfifo * fifo_pere= mfifo_connect("testBis",O_CREAT,0777,LEN);
		/*
		mfifo_disconnect("testBis");
		mfifo_unlink("testBis");*/
		
		//printf("Etat apres suppression\n" );
		//printf("Contenu du dossier /dev/shm/ : \n" );
		//execlp("ls","ls","/dev/shm/",NULL);
	/*
	}
	*/
	mfifo_disconnect("testBis");
	mfifo_unlink("testBis");


	return EXIT_SUCCESS;
}
					
