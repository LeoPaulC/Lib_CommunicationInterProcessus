
#include <stdio.h>
#include <stdlib.h>
#include "mfile.h"
#include <sys/mman.h>
#include <sys/stat.h> /* Pour les constantes « mode » */
#include <fcntl.h> /* Pour les constantes O_* */ 
#define LEN 512


void Init(void)
{
	printf("Projet System 2020 , TRAORE - CADIOU :\n\n");
}

mfifo *mfifo_connect( const char *nom, int options, mode_t permission, size_t capacite ){

	if ( !capacite || capacite == 0 ){
		perror("Capacité NULL ou = 0 , Erreur");
		exit(1);
	}
	mfifo * fifo = malloc(sizeof(mfifo)) ;
	/* debut fifo correspondant au retour de malloc */
	if ( nom != NULL ){
		fifo->nom = nom ;
	}
	else {
		/* mFifo anonyme */
		void * addr = mmap(&fifo, capacite, PROT_READ | PROT_WRITE,  MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
		fifo->debut = (size_t)&addr ;
		fifo->fin = fifo->debut + capacite ;
		fifo->capacity = capacite;
		fifo->pid = -1 ;
		printf("Creation tube anonyme a l'adresse suivante : %p avec une capacite de : %ld \n", &addr , capacite);
	    return fifo ;
	}
	/*
		La valeur du paramètre options est ignorée pour le mfifo anonyme.
		Pour un mfifo nommé, options peut prendre les valeurs suivantes :
			– 0 pour demander la connexion à un mfifo nommé existant,
			– O_CREAT pour demander la création de mfifo s’il n’existe pas, puis la connexion,
			– O_CREAT|O_EXCL pour indiquer qu’il faut créer un mfifo nommé seulement s’il
				n’existe pas : si mfifo existe mfifo_connect doit échouer.
	*/

	if ( nom != NULL ){
		int fd ;
		char * name = malloc(sizeof(char)*(sizeof(nom)+2));
		//printf("Option : %d\n", options );
		switch(options){
			case 0 :
				strcat(name,"/") ;
				strcat(name,nom);
				//printf("Connexion a un mfifo existant... Name : %s .\n",name);
				fd = shm_open(name, O_RDWR, permission);
				//printf("Valeur du fd retour de shm_open() : %d \n", fd );
				if ( fd != -1 ){
					void *addr = mmap(&fifo, capacite , PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
					//printf("Retour de mmap (pointeur): %p\n", &addr );
					//printf("Retour de mmap (long)	 : %ld\n", &addr );
		    		close(fd);

		    		//printf("TEST ECRITURE --------------\n");
					

		    		fifo->debut = (size_t)&addr ;
			    	fifo->fin = fifo->debut + capacite ;
			    	fifo->capacity = capacite;
			    	fifo->pid = -1 ;
	    			return fifo ;
	    		}
	    		else{
	    			return NULL ;
	    		}

			break ;



			case O_CREAT :
				//printf("Option : O_CREAT \n" );
				strcat(name,"/") ;
				strcat(name,nom);
				//printf("Creation d'un mfifo ... Name : %s .\n",name);
				if ( permission != 0  ){
					fd = shm_open(name, O_CREAT, permission );// cree avec permission
					//printf("Valeur du fd retour de shm_open() : %d , avec les permission suivantes : %d \n", fd , permission);
					if ( fd != -1 ){
						void *addr = mmap(&fifo, capacite , PROT_READ | PROT_WRITE, MAP_SHARED, fd, permission);
						//printf("Retour de mmap (pointeur): %p\n", &addr );
						//printf("Retour de mmap (long)	 : %ld\n", &addr );
		    			close(fd);

		    			fifo->debut = (size_t)&addr ;
			    		fifo->fin = fifo->debut + capacite ;
			    		fifo->capacity = capacite;
			    		fifo->pid = -1 ;
		    			return fifo ;
		    		}
		    		else{
		    			fd = shm_open(name, O_RDWR, permission );
		    			if ( fd != -1 ){
							void *addr = mmap(&fifo, capacite , PROT_READ | PROT_WRITE, MAP_SHARED, fd, permission);
							//printf("Retour de mmap (pointeur): %p\n", &addr );
							//printf("Retour de mmap (long)	 : %ld\n", &addr );
			    			close(fd);

			    			fifo->debut = (size_t)&addr ;
				    		fifo->fin = fifo->debut + capacite ;
				    		fifo->capacity = capacite;
				    		fifo->pid = -1 ;
			    			return fifo ;
			    		}

		    			//printf("O_create , NULL\n");
		    			return NULL ;
		    		}
				}
								
			break;



			case O_CREAT|O_EXCL :
				//printf("Option : O_CREAT|O_EXCL \n" );
				strcat(name,"/") ;
				strcat(name,nom);
				//printf("Creation d'un mfifo seulement s'il n'existe pas... Name : %s .\n",name);
				fd = shm_open(name, O_RDWR, 0);
				if ( fd != -1 ){ // mfifo existe deja donc Connect() doit echouer
					perror("mfifo_connect() echoue car l'objet existe deja .\n");
				}
				else{
					if ( permission != 0 ){
						fd = shm_open(name, O_CREAT, permission );// cree avec permission
						if ( fd == -1 ){
							perror("mfifo_connect() echoue car creation echoue .\n");
							exit(1);
						}else{

							void *addr = mmap(&fifo, capacite , PROT_READ | PROT_WRITE, MAP_SHARED, fd, permission);
							//printf("Retour de mmap (pointeur): %p\n", &addr );
							//printf("Retour de mmap (long)	 : %ld\n", &addr );
			    			close(fd);

			    			fifo->debut = (size_t)&addr ;
				    		fifo->fin = fifo->debut + capacite ;
				    		fifo->capacity = capacite;
				    		fifo->pid = -1 ;
			    			return fifo ;
						}
					}
					else {
						fd = shm_open(name, O_CREAT, 0 ); // cree sans permission
						if ( fd == -1 ){
							perror("mfifo_connect() echoue car creation echoue .\n");
							exit(1);
						}
						else{

							void *addr = mmap(&fifo, capacite , PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
							//printf("Retour de mmap (pointeur): %p\n", &addr );
							//printf("Retour de mmap (long)	 : %ld\n", &addr );
			    			close(fd);

			    			fifo->debut = (size_t)&addr ;
				    		fifo->fin = fifo->debut + capacite ;
				    		fifo->capacity = capacite;
				    		fifo->pid = -1 ;
			    			return fifo ;
						}

					}
				}
				

				//printf("Valeur du fd retour de shm_open() : %d , avec les permission suivantes : %d \n", fd , permission);
			break ;

		}

	}
	// TODO : gerer les mfifo anonyme


	/* 	Le paramètre permission est ignoré sauf à la création du shared memory object associé
		à un mfifo nommé (ouvert en mode O_CREAT et indique alors les permissions d’accès
		attribuées à cet objet (troisième paramètre de shm_open).
		La fonction mfifo_connect() retourne un pointeur vers un objet mfifo qui sera utilisé
		par d’autres opérations, voir section 4 pour la description du type mfifo.
		En cas d’échec, mfifo_connect() retourne NULL.
	*/
	return fifo ;
}

int write_addr(char *val , mfifo * fifo ) {

    printf("> Content : %s\n", val);
    
    printf(">> Ecriture / Lecture :\n");

    for ( int i = 0 ; i < (int)strlen(val) ; i++ ){
    	fifo->memory[i] = val[i] ;
    	printf("%c", ((char*) fifo->memory)[i]);

    } 
    return 0;
}

/**
* Cette fonction déconnect, rend inutilisable, un objet mfifo, retourn -1 en cas d'erreur sinon 0
*
* @param fifo	objet mfifo à rendre ne plus utiliser
*/
int mfifo_disconnect(mfifo *fifo){
	int r = munmap((void*)fifo->debut,fifo->capacity);
	if( r == -1){
		perror("shm unlink");
	}
	return r;
}

/**
* Cette fonction supprime un objet mfifo par son nom, retourn -1 en cas d'erreur sinon 0
*
* @param nom	nom de l'objet mfifo à supprimer
*/
int mfifo_unlink(const char*nom){
	int r = shm_unlink(nom);
	if( r == -1){
		perror("shm unlink");
	}
	return r;
}