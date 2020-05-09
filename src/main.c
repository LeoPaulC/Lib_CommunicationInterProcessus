
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "mfile.h"

#define LEN 150

int main(void)
{
	Init();
	//printf("------ Creation PUIS Connexion : -------\n");
	
	//mfifo * fifo = mfifo_connect("testBis",O_CREAT,0777,LEN);

	mfifo * fifo = mfifo_connect(NULL,O_CREAT,0777,LEN);

	char *s="Nous avons enfin un Test.";
	int status = 0 ;
	pid_t pid = fork();

	if ( pid == 0 ) {

		size_t lon = strlen(s);
		size_t long_tot = sizeof(message) + lon + 1;
		message *m = malloc(long_tot);
		m->l = lon + 1;
		memmove(m->mes, s, m->l) ;
		//printf("Len buf %d + en-tete : %d \n",  strlen(s), 8 + strlen(s));
		mfifo_write(fifo, m, long_tot);
		printf("Processus : %d | Il reste %ld places de libres dans le mfifo '%s'\n", getpid() , mfifo_free_memory(fifo), fifo->nom );
		
		//print_fifo_memory(fifo);

		//printf("\n\n----------Fils1 -------------- \n");
		/*mfifo * fifo_enfant = mfifo_connect("testBis",0,0777,LEN);
		
		size_t lon = strlen(s);
		size_t long_tot = sizeof(message) + lon + 1;
		message *m = malloc(long_tot);
		m->l = lon + 1;
		memmove(m->mes, s, m->l) ;
		//printf("Len buf %d + en-tete : %d \n",  strlen(s), 8 + strlen(s));
		mfifo_write(fifo_enfant, m, long_tot);
		printf("Processus : %d | Il reste %ld places de libres dans le mfifo '%s'\n", getpid() , mfifo_free_memory(fifo_enfant), fifo_enfant->nom );
		sleep(1);

		char * bufTest = "Deuxieme Test de message !" ;
		mfifo_write(fifo_enfant, bufTest , strlen(bufTest));
		printf("Processus : %d | Il reste %ld places de libres dans le mfifo '%s'\n", getpid() , mfifo_free_memory(fifo_enfant), fifo_enfant->nom );
		
		bufTest = "Un troisieme pour la route !" ;
		mfifo_write(fifo_enfant, bufTest , strlen(bufTest));
		printf("Processus : %d | Il reste %ld places de libres dans le mfifo '%s'\n", getpid() , mfifo_free_memory(fifo_enfant), fifo_enfant->nom );
		

		bufTest = "Quatrieme test avec un tryWrite pour voir." ;
		mfifo_trywrite(fifo_enfant, bufTest , strlen(bufTest));
		printf("Processus : %d | Il reste %ld places de libres dans le mfifo '%s'\n", getpid() , mfifo_free_memory(fifo_enfant), fifo_enfant->nom );
		check_return_errno();
		bufTest = "Ici nous allons essayer de mettre un texte trop long et donc tester le Errno." ;
		mfifo_trywrite(fifo_enfant, bufTest , strlen(bufTest));
		printf("Processus : %d | Il reste %ld places de libres dans le mfifo '%s'\n", getpid() , mfifo_free_memory(fifo_enfant), fifo_enfant->nom );
		check_return_errno();
		*/
		return 0 ;
		

		
	}
	else {

		//waitpid(pid,&status,0);
		wait(NULL);

		sleep(1);
		message *ps = malloc( sizeof( message) );
		mfifo_read(fifo, ps, sizeof(message));
		//printf("Contenu lu : %s \n", ps->l );
		ps = realloc( ps, ps->l + sizeof(message) );
		printf("On va lire un message de taille : %d \n", ps );
		mfifo_read(fifo, ps->mes , ps->l-1 );
		print_fifo_memory(fifo);
		printf("\nProcessus : %d | Message lu : '%s'\n", getpid() , ps->mes  );	
		

		/*sleep(1);
		//printf("------------------- Pere ---------------\n");
		mfifo * fifo_pere= mfifo_connect("testBis",0,0777,LEN);
		printf("Processus : %d | Il reste %ld places de libres dans le mfifo '%s'\n", getpid() , mfifo_free_memory(fifo_pere), fifo_pere->nom );
		
		message *ps = malloc( sizeof( message) );
		mfifo_read(fifo_pere, ps, sizeof(message));
		ps = realloc( ps, ps->l + sizeof(message) );
		//printf("On va lire un message de taille : %d \n", ps->l );
		mfifo_read(fifo_pere, ps->mes , ps->l-1 );
		printf("\nProcessus : %d | Message lu : '%s'\n", getpid() , ps->mes  );	

		sleep(1) ; 

		char * buf = malloc(30) ;
		mfifo_read(fifo_pere, buf , 30 );
		printf("\nProcessus : %d | Buffer[30] lu : '%s'\n",getpid() , buf  );	

		sleep(1) ; 

		buf = malloc(30) ;
		mfifo_read(fifo_pere, buf , 30 );
		printf("\nProcessus : %d | Buffer[30] lu : '%s'\n", getpid() , buf  );

		buf = malloc(30) ;
		mfifo_read(fifo_pere, buf , 30 );
		printf("\nProcessus : %d | Buffer[30] lu : '%s'\n", getpid() , buf  );	


		waitpid(pid,&status,0);
		mfifo_unlink("testBis");
		mfifo_disconnect(fifo);
		*/
		
	}
	
	
	return EXIT_SUCCESS;
}
					
