
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "mfile.h"

#define LEN 2048

int main(void)
{
	Init();
	printf("Nous allons travailler avec des mfifo de taille : %d\n", LEN );

	mfifo * fifoAnonyme = mfifo_connect(NULL,O_CREAT,0777,LEN);

	mfifo * fifoNomme = mfifo_connect("testBis",O_CREAT,0777,LEN);

	char *s= "Voici un test de message ( adapte a la structure message ).";
	int status = 0 ;
	printf("\n------------------------------------------------------------------------------------");
	printf("\n -- Voici un test d'ecriture / lecture de mfifo ANONYME entre un pere et son fils --\n");
	printf("------------------------------------------------------------------------------------\n\n");
	pid_t pid = fork();

	if ( pid == 0 ) {
		/*
		On va créer un message et ecrire dans un mfifo anonyme pour tester la communication inter_processus.
		*/
		size_t lon = strlen(s);
		size_t long_tot = sizeof(message) + lon + 1;
		message *me = malloc(long_tot);
		me->l = lon + 1;
		memmove(me->mes, s, me->l) ;
		mfifo_write(fifoAnonyme, me, long_tot);
		printf("Processus : %d | Fifo anonyme | Il reste %ld places de libres dans le mfifo '%s'\n", getpid() , mfifo_free_memory(fifoAnonyme), fifoAnonyme->nom );
		return 0 ;
	}
	else {
		waitpid(pid,&status,0);
		message *ps = malloc( sizeof( message) );
		mfifo_read(fifoAnonyme, ps, sizeof(message));
		//printf("Contenu lu : %s \n", ps->l );
		ps = realloc( ps, ps->l + sizeof(message) );
		printf("\nOn va lire un message de taille : %ld dans le fifoAnonyme.\n", ps->l );
		mfifo_read(fifoAnonyme, ps->mes , ps->l-1 );
		print_fifo_memory(fifoAnonyme);
		//printf("\nProcessus : %d | Message lu : '%s'\n", getpid() , ps->mes  );	
		mfifo_disconnect(fifoAnonyme);
	}
	printf("\n------------------------------------------------------------------------------------");
	printf("\n -- Voici un test d'ecriture / lecture de mfifo NOMME entre un pere et son fils --\n");
	printf("------------------------------------------------------------------------------------\n\n");

	pid = fork();

	if ( pid == 0 ) {

		mfifo * fifo_enfant = mfifo_connect("testBis",0,0777,LEN);

		char *s= "Un deuxieme test pour verifier l'ecriture et le lecture dans un mfifo nommé entre deux processus.";
		
		size_t lon = strlen(s);
		size_t long_tot = sizeof(message) + lon + 1;
		message *m = malloc(long_tot);
		m->l = lon + 1;
		memmove(m->mes, s, m->l) ;
		mfifo_write(fifo_enfant, m, long_tot);

		printf("Processus : %d | Il reste %ld places de libres dans le mfifo '%s'\n", getpid() , mfifo_free_memory(fifo_enfant), fifo_enfant->nom );
		
		return 0 ;
	}
	else {

		waitpid(pid,&status,0);
		mfifo * fifo_pere= mfifo_connect("testBis",0,0777,LEN);
		printf("Processus : %d | Il reste %ld places de libres dans le mfifo '%s'\n", getpid() , mfifo_free_memory(fifo_pere), fifo_pere->nom );
		message *ps = malloc( sizeof( message) );
		mfifo_read(fifo_pere, ps, sizeof(message));
		ps = realloc( ps, ps->l + sizeof(message) );
		mfifo_read(fifo_pere, ps->mes , ps->l-1 );
	}
	printf("\n------------------------------------------------------------------------------------");
	printf("\n -- Voici une boucle d'ecriture et lecture de mfifo NOMME entre un pere et 2 fils -\n");
	printf("------------------------------------------------------------------------------------\n\n");
	pid = fork() ;
	if ( pid == 0 ){
		mfifo * fifo_enfant = mfifo_connect("TestBoucle",O_CREAT  ,0777,LEN);
		printf("\n");
		for ( int i = 0 ; i < 50 ; i ++ ){
			usleep(2) ;
			char * message = "Test numero ";
			char * b = malloc(2);
			//printf("Mess : %s \n" , message);
			sprintf(b , "%d" , i) ;
			//printf("B : %s \n", b );
			char * buf = malloc ( strlen(message) + 2 ) ;
			strcat(buf,message);
			strcat(buf,b);
			mfifo_write(fifo_enfant, buf, strlen(buf));
			//printf("Processus : %d | Il reste %ld places de libres dans le mfifo '%s'\n", getpid() , mfifo_free_memory(fifo_enfant), fifo_enfant->nom );
		}
		return 0 ;
}
	else {
		pid = fork() ;

		if ( pid == 0 ){
			//usleep(2);
			mfifo * fifo_pere = mfifo_connect("TestBoucle",0  ,0777,LEN);
			//printf("Processus : %d | Il reste %ld places de libres dans le mfifo '%s'\n", getpid() , mfifo_free_memory(fifo_pere), fifo_pere->nom );
			//printf("On va lire des buffer de taille 10.\n");
			char * buffer = malloc(10);
			for ( int i = 0 ; i < 25 ; i++ ){
				printf("\n>-- FILS --");
				mfifo_read(fifo_pere, buffer , 10 );
				//printf("\nPID %d | Message lu : '%s'\n", getpid() , buffer  );	
				//printf("******************\n" );
				usleep(3) ;
			}
			return 0 ;
		}
		else{
			mfifo * fifo_pere = mfifo_connect("TestBoucle",0 ,0777,LEN);
			char * buffer = malloc(10);
			for ( int i = 0 ; i < 25 ; i++ ){
				printf("\n>-- PERE --");
				mfifo_read(fifo_pere, buffer , 10 );
				//printf("\nPID %d | Message lu : '%s'\n", getpid() , buffer  );	
				usleep(3) ;
			}

			waitpid(pid,&status,0);
			mfifo_disconnect(fifo_pere);
		}
		printf("\n\n");
		sleep(1);
		mfifo * fifo_enfant = mfifo_connect("TestBoucle",0,0777,LEN);
		mfifo_disconnect(fifo_enfant);
	}

	printf("\n------------------------------------------------------------------------------------");
	printf("\n -- Test d'ecriture et lecture de mfifo NOMME entre un pere et un fils ( TRY WRITE )-\n");
	printf("------------------------------------------------------------------------------------\n\n");
	
	mfifo * test_trywrite = mfifo_connect("test_trywrite",O_CREAT,0777,20);
	pid = fork() ;
	if ( pid == 0 ){
		char * buf = "Un test de TryWrite avec une longueur superieur a Len" ;
		mfifo_trywrite(test_trywrite, buf, strlen(buf));
		check_return_errno();

		buf = "Un test de TryWrite" ;
		mfifo_trywrite(test_trywrite, buf, strlen(buf));

		printf("** On essaie d'ecrire un message qui ne pourras pas etre inséré. **\n");
		buf = "Deuxieme test" ;
		mfifo_trywrite(test_trywrite, buf, strlen(buf));
		check_return_errno();
		return 0 ;
	}
	else {
		waitpid(pid,&status,0);		
		char * buffer = malloc(10);
		mfifo_read(test_trywrite, buffer , 10 );
	}

	printf("\n------------------------------------------------------------------------------------");
	printf("\n -- Test d'ecriture et lecture de mfifo NOMME entre un pere et un fils ( WRITE PARTIAL )-\n");
	printf("------------------------------------------------------------------------------------\n\n");


	mfifo * test_writepartial = mfifo_connect("test_writepartial",O_CREAT,0777,10);
	pid = fork() ;

	if ( pid == 0 ){
		char * buf = "Un test de WritePartial avec une longueur superieur a Len" ;
		mfifo_write_partial(test_writepartial, buf, strlen(buf));
		check_return_errno();
		sleep(1);
		return 0 ;
	}
	else {
		sleep(1);
		printf("**On va essayer de lire des paquets de 10 octets pour liberer de la place au Write_Partial ** \n");
		char * buffer = malloc(10);
		for ( int i = 0 ; i < 6 ; i++ ){
			mfifo_read(test_writepartial, buffer , 10 );
			sleep(1);
			buffer = malloc(10);
		}
	}	


	
	
	printf("\n");
	mfifo_unlink("TestBoucle");
	mfifo_unlink("testBis");
	mfifo_disconnect(fifoNomme);
	mfifo_unlink("test_trywrite");
	mfifo_disconnect(test_trywrite);
	mfifo_unlink("test_writepartial");
	mfifo_disconnect(test_writepartial);
	return EXIT_SUCCESS;
}
					
