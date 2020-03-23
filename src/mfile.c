
#include <stdio.h>
#include <stdlib.h>
#include "mfile.h"
#include <sys/mman.h>
#include <sys/stat.h> /* Pour les constantes « mode » */
#include <fcntl.h> /* Pour les constantes O_* */ 


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
	if ( nom != NULL )
		fifo->nom = nom ;
	/*
	else {
		mfifo anonyme ;
	}
	*/

	fifo->debut = (size_t)fifo ;
	printf("Debut du pointeur fifo : %ld \n", fifo->debut );
	/* capacity est definie par la capacité fournie en arg. */
	fifo->capacity = (size_t)capacite ;
	printf("Cap. fifo : %ld \n" , fifo->capacity);
	/* fin correspond a debut+ cap. s*/
	fifo->fin = fifo->debut+fifo->capacity ;
	printf("Fin du pointeur fifo : %ld \n", fifo->fin );

	/*
		La valeur du paramètre options est ignorée pour le mfifo anonyme.
		Pour un mfifo nommé, options peut prendre les valeurs suivantes :

			– 0 pour demander la connexion à un mfifo nommé existant,
			– O_CREAT pour demander la création de mfifo s’il n’existe pas, puis la connexion,
			– O_CREAT|O_EXCL pour indiquer qu’il faut créer un mfifo nommé seulement s’il
				n’existe pas : si mfifo existe mfifo_connect doit échouer.

	*/

	/*
	int fd = shm_open("/liste", O_RDWR, 0);
    struct stat buf_stat;
    if (fstat(fd, &buf_stat) == -1) {
        exit(1);
    }

    void *addr =
        mmap(NULL, buf_stat.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);



	*/

	if ( nom != NULL ){
		int fd ;
		char * name = malloc(sizeof(char)*(sizeof(nom)+2));
		printf("Option : %d\n", options );

		switch(options){

			case 0 :
				strcat(name,"/") ;
				strcat(name,nom);
				printf("Connexion a un mfifo existant... Name : %s .\n",name);
				fd = shm_open(name, O_RDWR, 0);
				printf("Valeur du fd retour de shm_open() : %d \n", fd );

			break ;

			case O_CREAT :
				printf("Option : O_CREAT \n" );
				strcat(name,"/") ;
				strcat(name,nom);
				printf("Creation d'un mfifo ... Name : %s .\n",name);
				if ( permission != 0  )
					fd = shm_open(name, O_CREAT, permission );// cree avec permission
				else {
					fd = shm_open(name, O_CREAT, 0 ); // cree sans permission
				}
				printf("Valeur du fd retour de shm_open() : %d , avec les permission suivantes : %d \n", fd , permission);
			break;

			case O_CREAT|O_EXCL :
				printf("Option : O_CREAT|O_EXCL \n" );
				strcat(name,"/") ;
				strcat(name,nom);
				printf("Creation d'un mfifo seulement s'il n'existe pas... Name : %s .\n",name);
				fd = shm_open(name, O_RDWR, 0);
				if ( fd != -1 ){ // mfifo existe deja donc Connect() doit echouer
					perror("mfifo_connect() echoue car l'objet existe deja .\n");
				}
				else{
					if ( permission != 0 )
						fd = shm_open(name, O_CREAT, permission );// cree avec permission
					else {
						fd = shm_open(name, O_CREAT, 0 ); // cree sans permission
					}
				}

				printf("Valeur du fd retour de shm_open() : %d , avec les permission suivantes : %d \n", fd , permission);
			break ;

		}

	}
		
	/*
	else {
		mfifo anonyme ;
	}
	*/



	/* 	Le paramètre permission est ignoré sauf à la création du shared memory object associé
		à un mfifo nommé (ouvert en mode O_CREAT et indique alors les permissions d’accès
		attribuées à cet objet (troisième paramètre de shm_open).
		La fonction mfifo_connect() retourne un pointeur vers un objet mfifo qui sera utilisé
		par d’autres opérations, voir section 4 pour la description du type mfifo.
		En cas d’échec, mfifo_connect() retourne NULL.
	*/




	return fifo ;
}
