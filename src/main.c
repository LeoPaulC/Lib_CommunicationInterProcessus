
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mfile.h"

#define LEN 150

int main(void)
{
	Init();
	printf("------ Creation PUIS Connexion : -------\n");
	
	mfifo * fifo = mfifo_connect("testBis",O_CREAT,0777,LEN);
	char* buf = "Nous avons enfin un test concluant Chef :)" ;

	message * m = malloc(sizeof(message) + strlen(buf)+1) ;

	create_message(buf,m) ;

	int r = mfifo_write(fifo,m,m->l+sizeof(message)+1);
	printf("Main .. | Ecriture 1 memory : %s\n",&fifo->memory[0] );
	printf("Main .. | Ecriture 1 memory : %s\n",&fifo->memory[8] );

	printf("\nTest d'une seconde ecriture pour evrifier que le processus attends bien de la place.\n\n");


	char* b = "Et voici un deuxieme test , pour le plaisir des yeux !" ;

	message * ms = malloc(sizeof(message) + strlen(b)+1) ;

	create_message(b,ms) ;

	int rbis = mfifo_write(fifo,ms,ms->l+sizeof(message)+1);
	

	buf = malloc(m->l+sizeof(message)+1) ;
	printf("Main | Nous vidons bien le Buff pour etre sur de nos test : \n\tBuf : %s \n", buf  );

// Len definie temporairement pour nos test
	size_t resRead = mfifo_read_message(fifo, buf, 0 ) ; // la taille n'importeplus grace au systeme de message
	printf("Main | A la sortie de la Lecture nous avons : %s \n", buf );
	printf("Main | et le contenue de fifo est maintenant : \n>%s\n" , &fifo->memory[0] );

	printf("Main | Au Retour du Read , Buf : %s \n" , buf );
	
	int status = 0 ;
	
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
		return EXIT_SUCCESS ;
	}
	else {
		//sleep(2);
		waitpid(pid , &status , 0);
		printf("\n\n------------ Pere ---------------\n");
		mfifo * fifo_pere= mfifo_connect("testBis",O_CREAT,0777,LEN);
		
		mfifo_disconnect("testBis");
		mfifo_unlink("testBis");

	}
	return EXIT_SUCCESS;
}
					
