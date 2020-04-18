
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <errno.h>
#include "mfile.h"
#include <errno.h>
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
		void * addr = mmap(NULL, capacite, PROT_READ | PROT_WRITE,  MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
		if (addr == MAP_FAILED){
            perror("mmap");
           	exit(1);
        }
		fifo->debut = (size_t)addr ;
		fifo->fin = fifo->debut + capacite ;
		fifo->capacity = capacite;
		fifo->pid = -1 ;
		sem_init(&fifo->sem,1,0);
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
					void *addr = mmap(NULL, capacite , PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

					if (addr == MAP_FAILED){
			            perror("mmap");
			           	exit(1);
			        }
					//printf("Retour de mmap (pointeur): %p\n", &addr );
					//printf("Retour de mmap (long)	 : %ld\n", &addr );
		    		close(fd);

		    		//printf("TEST ECRITURE --------------\n");
					
					fifo->debut = (size_t)addr ;
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
					fd = shm_open(name, O_CREAT | O_RDWR , permission );// cree avec permission
					//printf("Valeur du fd retour de shm_open() : %d , avec les permission suivantes : %d \n", fd , permission);
					if ( fd != -1 ){
						void *addr = mmap(NULL, capacite , PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
						//printf("Retour de mmap (pointeur): %p\n", &addr );
						//printf("Retour de mmap (long)	 : %ld\n", &addr );
						if (addr == MAP_FAILED){
				            perror("O_creat , mmap");
				           	exit(1);
			        	}

			        	//int r = munmap( addr , capacite );
						//printf("Retour munmap : %d \n" , r );

		    			close(fd);
		    			fifo->debut = (size_t)addr ;
			    		fifo->fin = fifo->debut + capacite ;
			    		fifo->capacity = capacite;
			    		fifo->pid = -1 ;
			    		sem_init(&fifo->sem,1,0);
			    		/* 1 -> partagée entre != processus 
			    		   0-> valeur initiale
			    		   */
		    			return fifo ;
		    		}
		    		else{
		    			fd = shm_open(name, O_RDWR, permission );
		    			if ( fd != -1 ){
							void *addr = mmap(NULL, capacite , PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
							//printf("Retour de mmap (pointeur): %p\n", &addr );
							//printf("Retour de mmap (long)	 : %ld\n", &addr );
							if (addr == MAP_FAILED){
					            perror("mmap");
					           	exit(1);
					        }
			    			close(fd);

			    			fifo->debut = (size_t)addr ;
				    		fifo->fin = fifo->debut + capacite ;
				    		fifo->capacity = capacite;
				    		fifo->pid = -1 ;
				    		sem_init(&fifo->sem,1,0);
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

							void *addr = mmap(NULL, capacite , PROT_READ | PROT_WRITE, MAP_SHARED, fd, permission);
							//printf("Retour de mmap (pointeur): %p\n", &addr );
							//printf("Retour de mmap (long)	 : %ld\n", &addr );
			    			close(fd);

			    			fifo->debut = (size_t)addr ;
				    		fifo->fin = fifo->debut + capacite ;
				    		fifo->capacity = capacite;
				    		fifo->pid = -1 ;
				    		sem_init(&fifo->sem,1,0);
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

							void *addr = mmap(NULL, capacite , PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
							//printf("Retour de mmap (pointeur): %p\n", &addr );
							//printf("Retour de mmap (long)	 : %ld\n", &addr );
			    			close(fd);

			    			fifo->debut = (size_t)addr ;
				    		fifo->fin = fifo->debut + capacite ;
				    		fifo->capacity = capacite;
				    		fifo->pid = -1 ;
				    		sem_init(&fifo->sem,1,0);
			    			return fifo ;
						}

					}
				}
			
			break ;

		}

	}
	
	return fifo ;
}


	/*
	mfifo_write() bloque le processus appelant jusqu’à ce que len octets soient écrits dans
	fifo. Les octets écrits ne doivent pas être mélangés avec les octets écrits par d’autres
	processus, et le processus appelant reste donc bloqué tant qu’il n’y a pas de place pour
	len octets dans fifo.
	*/
int mfifo_write(mfifo *fifo, const void *val, size_t len){
	printf("Dans mfifo_write : \n");
	printf("Len : %ld\n",len );
	// On test que LEN est bien < fifo->capacite .
	if ( len > fifo->capacity){
		perror("Erreur , Len > fifo->capacite\n");
		errno = EMSGSIZE;
		return(-1);
	}
    printf("> Content : %s\n", (char*)val);
    printf("> Ecriture : \n");
    // on copie len octets dans fifo->memory
    memcpy( fifo->memory, val , len );
   	//printf("%s", fifo->memory);
    
    return 0;
}

/*
Supposons qu’un mfifo contient n octets et l = min(len, n). La fonction mfifo_read()
copie l octets de mfifo à l’adresse buf. Les octets copiés sont supprimés du mfifo et
mfifo_read retourne le nombre d’octets copiés.
S’il y plusieurs processus lecteurs qui tentent de lire en même temps, tous sauf un seront
bloqués en attendant la fin de la lecture du seul processus autorisé à lire. Chaque lecture
lit donc un segment contigu d’octets stockés dans le mfifo.
*/
ssize_t mfifo_read(mfifo *fifo, void *buf, size_t len){
	int count = 0 ;
	printf("Dans mfifo_read : \n");
	printf("Len : %ld\n",len );
	printf("Buf : %s\n> ",  (char*)buf );
	
	for ( int i = 0 ; i < (int)len ; i++ ){
		memcpy( &buf[i], &fifo->memory[i] , 1 );
	    memset(&fifo->memory[i],0,1);
	    count++ ;
	}
	printf("%s", (char*)buf);

    return count;

}

/**
* Cette fonction déconnect, rend inutilisable, un objet mfifo, retourn -1 en cas d'erreur sinon 0
*
* @param fifo	objet mfifo à rendre ne plus utiliser
*/
int mfifo_disconnect(mfifo *fifo){
	printf("dans disconnect \n");
	printf("fifo debut : %ld \n", fifo->debut );
	printf("Cap. fifo : %ld \n" , fifo->capacity);
	int r = munmap( (void*)fifo->debut , fifo->capacity );
	printf("Retour munmap : %d \n" , r );
	return r;
}

/**
* Cette fonction supprime un objet mfifo par son nom, retourn -1 en cas d'erreur sinon 0
*
* @param nom	nom de l'objet mfifo à supprimer
*/
int mfifo_unlink(const char * nom){
	printf("dans unlink : nome := %s \n",nom );
	int r = shm_unlink(nom);
	if( r == -1){
		perror("shm unlink");
	}
	return r;
}