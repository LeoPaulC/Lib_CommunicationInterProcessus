
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

	char* buf = "Nous avons enfin un test concluant Chef :)" ;

	message * m = malloc(sizeof(message) + strlen(buf)+1) ;

	create_message(buf,m) ;

	int r = mfifo_write(fifo,m,m->l+sizeof(message)+1);
	//printf("fifo cap main %s\n",fifo->memory );

	buf = malloc(m->l+sizeof(message)+1) ;
	printf("Main | Nous vidons bien le Buff pour etre sur de nos test : \n\tBuf : %s \n", buf  );

// Len definie temporairement pour nos test
	size_t resRead = mfifo_read(fifo, buf, m->l+sizeof(message)+1) ;

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
		
		//printf("Etat apres suppression\n" );
		//printf("Contenu du dossier /dev/shm/ : \n" );
		//execlp("ls","ls","/dev/shm/",NULL);

	}
	return EXIT_SUCCESS;
}
					
