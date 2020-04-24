
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <errno.h>
#include "mfile.h"
#include <errno.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/stat.h> /* Pour les constantes « mode » */
#include <fcntl.h> /* Pour les constantes O_* */ 
#define LEN 512


void Init(void)
{
	printf("Projet System 2020 , TRAORE - CADIOU :\n\n");
}

mfifo * mfifo_connect( const char *nom, int options, mode_t permission, size_t capacite ){

	if ( !capacite || capacite == 0 ){
		perror("Capacité NULL ou = 0 , Erreur");
		return NULL;
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
           	return NULL;
        }
		fill_mfifo(fifo,(size_t) addr, capacite,NULL);
	    return fifo ;
	}
	if ( nom != NULL ){
		int fd ;
		char * name = malloc(sizeof(char)*(sizeof(nom)+2));
		strcat(name, "/");
		strcat(name, nom);
		switch(options){
			case 0 :
				printf("\nCONNEXION ....\n");
				if( permission !=0){
					mfifo * res = connexion_mfifo_nomme(name, capacite, permission); 
					if(  res == NULL){
						printf("echec de connexion \n");
						return NULL;
					}
					//printf("Contenu memoire au retour de conecxion nomme  : %s\n", res->memory );
					return res ;
				}
				break;

			case O_CREAT :
			
				if ( permission != 0  ){
					mfifo * res = creation_mfifo_nomme( name, capacite, permission);
					if( res == NULL){
						res = connexion_mfifo_nomme(name, capacite, permission);
						if( res == NULL ){
							//printf("echec de conne\n");
							return NULL;
						}
					}
					return res;
				}					
				break;

			case O_CREAT|O_EXCL :

				if( permission != 0){
					//printf("excl\n");
					mfifo * res = creation_mfifo_nomme( name, capacite, permission);
					//printf("fin create\n");
					if( res == NULL){
						//printf("echec de création \n");
						return NULL;
					}
					return res;
				}
				break;
		}
	}
	return fifo ;
}

/**
* Gère la connexion d'un mfifo nommé
*
*/
mfifo * connexion_mfifo_nomme(char * name, size_t capacite, mode_t permission){
	mfifo * res = malloc(sizeof(mfifo)+capacite);
	int fd = shm_open(name, O_RDWR, permission);
	if( fd == -1 ){
		perror("shm open ");
		exit(1);
	}
	struct stat buf_stat;
	if (fstat(fd, &buf_stat) == -1) {
    	perror(" fstat ");
    	exit(1);
    }
	res = mmap(NULL, capacite, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	if( res == MAP_FAILED){
		//mfifo_unlink(name);
		perror("O - mmap");
		exit(1);
	}
    close(fd);
	printf("Connexion Fifo | adresse du fifo : %p\n", res );
	//printf("Contenu memoire a la connexion : %s\n", res->memory );
    fill_mfifo(res,(size_t) res , capacite, name);
   	printf("Connexion Fifo | Contenu memoire a la connexion : %s\n", res->memory );

    return res;
}

mfifo * creation_mfifo_nomme(char * name, size_t capacite, mode_t permission){
	printf("Creation Fifo | dans creation nomme\n");
	mfifo * res = malloc(sizeof(mfifo)+capacite);
	//printf("malloc\n");	
	int fd = shm_open(name, O_CREAT | O_RDWR | O_EXCL, permission );
	if(fd == -1){
		perror("shm open ");
		return NULL;
	}
	struct stat buf_stat;
	if (fstat(fd, &buf_stat) == -1) {
		perror(" fstat ");
    	exit(1);
	}
	//printf("stat\n");
	if( buf_stat.st_size == 0 && ftruncate( fd, capacite ) == -1){
		perror("Ftruncate");
		exit(1);
	}
	//printf("truncate\n");
  	res = mmap(NULL, capacite, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	if( res == MAP_FAILED){
	    //mfifo_unlink(name);
	    perror("O_CREAT - mmap");
	    exit(1);
  	}
	close(fd);
	printf("Creation Fifo | Dans create fifo_Vide , adresse du fifo : %p\n", &res[0] );
	fill_mfifo(res,(size_t) res , capacite,name);
	init_memory_mfifo(res);

	return res;
}

/**
* Remplie un objet mfifo par diverses 
*
* @param fifo 		objet mfifo à remplir
* @param addr 		adresse de début de fifo
* @param capacite 	capacité totale de fifo
*/
void fill_mfifo(mfifo * fifo, size_t addr, size_t capacite, char *name){
	
	if(name == NULL){
		fifo->nom = NULL;
	}else{
		fifo->nom = malloc(strlen(name));
		memcpy(fifo->nom, name, strlen(name));
	}

	fifo->debut = addr ;
	fifo->fin = fifo->debut + capacite ;
	fifo->capacity = capacite;
	fifo->pid = -1 ;

	if(sem_init(&fifo->sem,1,1) == -1 ){
		perror("sem init ");
	}
}

/**
* Initialise la memoire d'un mfifo
*
* @param fifo objet fifo dont il faut initialiser la memoire
*/
void init_memory_mfifo(mfifo * fifo){
	memset(fifo->memory, 0, strlen(fifo->memory));
}

int mfifo_trywrite(mfifo *fifo, const void *buf, size_t len){
	printf("Try_Write | val : : %s \n", buf );
	// On test que LEN est bien < fifo->capacite .
	if ( len > fifo->capacity ){
		perror("Try_Write | Erreur , Len > fifo->capacite\n");
		printf("Try_Write | Len : %d, fifo->capacity : %d, cpt : %d \n",len,fifo->capacity, mfifo_free_memory(fifo) );
		errno = EMSGSIZE;
		return -1;
	}
	int dispo = mfifo_free_memory(fifo) ;
	if ( len > dispo ){
		printf("Try_Write | On attend que de la palce se libere pour ecrire dans le fifo.\n");
		errno = EAGAIN ;
		return -1 ;
	}
	message * m = buf ;
	printf("Try_Write | Len du message : %d \n", m->l);
	printf("Try_Write | Content du message : %s \n", m->mes );

	size_t index = fifo->capacity - dispo ;
	
	printf("Try_Write | On va ecrire a l'octets n° %d dans le fifo .\n", index );

	char * bufLen = malloc(8);

	sprintf(bufLen, "%d", m->l);

	memcpy(&fifo->memory[index] , bufLen , 8) ;
	memcpy(&fifo->memory[index+8] , m->mes , m->l ) ;

	return 0;
}
	
int mfifo_write(mfifo *fifo, const void *val, size_t len){
	// On test que LEN est bien < fifo->capacite .
	if ( len > fifo->capacity ){
		perror("Write | Erreur , Len > fifo->capacite\n");
		printf("Write | Len : %d, fifo->capacity : %d, cpt : %d \n",len,fifo->capacity, mfifo_free_memory(fifo) );
		errno = EMSGSIZE;
		return -1;
	}
	int dispo = fifo->capacity - mfifo_free_memory(fifo) ;

	printf("Write | dispo : %d \n", dispo );

	while ( len > dispo ){
		printf("Write | On attend que de la palce se libere pour ecrire dans le fifo.\n");
		dispo = mfifo_free_memory(fifo) ;
	}
	message * m = val ;
	printf("Write | Len du message : %d \n", m->l);
	printf("Write | Content du message : %s \n", m->mes );

	int index = fifo->capacity - dispo ;
	
	printf("Write | On va ecrire a l'octets n° %ld dans le fifo .\n", fifo->capacity - dispo );

	char * bufLen = malloc(8);

	sprintf(bufLen, "%d", m->l);

	memcpy(&fifo->memory[fifo->capacity-dispo] , bufLen , 8) ;
	memcpy(&fifo->memory[fifo->capacity-dispo+8] , m->mes , m->l ) ;

	printf("Write | memory[%d] : %s \n", fifo->capacity - dispo,&fifo->memory[fifo->capacity-dispo]);
	printf("Write | memory[%d] : %s \n", fifo->capacity - dispo +8 ,&fifo->memory[fifo->capacity-dispo+8]);
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

/*
Permet la lecture par message .
*/ 
size_t mfifo_read_message(mfifo *fifo, void *buf, size_t len) {
	// on bloque le fifo
	mfifo_lock(fifo) ;

	message *ps = malloc( sizeof( message) );

	mfifo_read(fifo, ps, sizeof(message)); /* lire sans tableau mes[] */

	ps = realloc( ps, ps->l + sizeof(message) ); /* agrandir la mémoire pour le tableau */

	ps->l = mfifo_read(fifo, ps , ps->l ); /* lire le message lui même */
	//printf("Read 2 passe \n");

	printf("Read | Len message : %d , content message : %s \n", ps->l , ps->mes );

	memcpy(buf,ps->mes,ps->l) ;

	printf("Buf : %s \n", buf  );

	mfifo_unlock(fifo); // on debloque

}
print_fifo_memory(mfifo * fifo ){
	for ( int i = 0 ; i < fifo->capacity ; i++ ){
		int jmp = strlen(&fifo->memory[i]) ;
		printf("%s", &fifo->memory[i] );
		i += jmp ;
	}
	printf("\n");
}

/*
Permet une lecture ( 2 appel necessaire pour lire un message )
*/
ssize_t mfifo_read(mfifo *fifo, void *buf, size_t len){
	
	memcpy(buf , &fifo->memory[0] , len ) ;
	if ( atoi(buf) > 0 ){
		message * ms = buf ;
		ms->l = atoi(buf) ;
		printf("Read | taille du message a suivre : %d \n", ms->l );
		//printf("\nOn a fixé la taille du message à ms->l : %d \n", ms->l );
		memcpy(&fifo->memory[0] , &fifo->memory[len] , fifo->capacity ) ;
		//printf("decalage passe de %d octets \n" , len );
	}
	else{
		message * ms = buf ;
		memcpy(&ms->mes[0] , &fifo->memory[0] , len ) ;
		printf("Read | Content du message : %s  \n", ms->mes );
		memcpy(&fifo->memory[0] , &fifo->memory[len] , fifo->capacity) ;
		//printf("decalage passe de %d octets \n" , len );
	}
	return len ;

}


/**
* Verrouille le mfifo pour la lecture, fonction bloquante, attend tend que 
* le verrou n'a pas été posé sur l'objet mfifo
*
* @param fifo objet mfifo sur lequel nous voulons poser un verrou
* @return -1 en cas d'echec , 0 sinon
*/
int mfifo_lock(mfifo *fifo){
	/*ajout du pid du processus appelant dans la queue de la semaphore*/
	if(sem_wait(&fifo->sem) == -1){
		perror("sem wait ");
		return -1;
	}
	return 0;
}

/**
*	Essais de vérouiller l'objet mfifo
*
* @param 	fifo objet mfifo sur lequel nous voulons essayer de poser un verrou
* @return 	revoie 0 si l'obtention du verrou réussi, -1 sinon
*/
int mfifo_trylock(mfifo *fifo){
	if(sem_trywait(&fifo->sem) == -1){
		perror("sem trywait");
		return -1;
	}
	return 0;
}

/**
* Déverrouille l’accès au mfifo en lecture
*
* @param fifo 	objet mfifo dont il faut lever un verrou
* @return     	renvoie 0 lors de la libération du verrou , -1 en cas d'échec
*/
int mfifo_unlock(mfifo *fifo){
	if(sem_post(&fifo->sem) == -1 ){
		perror("sem post  ");
		return -1;
	}
	return 0;
}

/*
* Informe de la capacité totale de l'objet mfifo
*
* @param fifo 	objet mfifo dont l'on savoir la capacité de stockage
* @return 		renvoie la capicité totale de stockage de fifo
*/
size_t mfifo_capacity(mfifo *fifo){
	return fifo->capacity;
}

/*
* Libere la mémoire allouer par l'objet fifo et ses diverses champs
*
* @param 	fifo ojbet mfifo dont l'on veut libérer la mémoire
* @return 	renvoie la taille du fifo apres libération de mémoire
*/
int free_mfifo(mfifo *fifo){
	//printf("dans free mfifo\n");
	free(&fifo->nom);
	if( strlen(fifo->memory) !=0 ){
		free(fifo->memory);
	}
	if(sem_destroy(&fifo->sem) == -1){
		perror("destroy semaphore ");
		return -1;
	}
	return sizeof(fifo);
}

/**
* Renvoie la quantié de mémoire libre de l'objet mfifo
*
* @param fifo 	objet mfifo dont l'on veut savoir la quantité de mémoire libre
* @return 		renvoie la quantité de mmémoire libre de fifo
*/
size_t mfifo_free_memory(mfifo *fifo){
	/* TODO */
	int occupe = 0 ;
	int libre = fifo->capacity ;
	int cpt = 0 ;
	for ( int i = 0 ;  i < (int)fifo->capacity ; i++ )
	{
		char * tmp = malloc(8) ;
		memcpy(tmp , &fifo->memory[i] , 8) ;
		int LenTmp = atoi(tmp);
		if ( LenTmp > 0 ) {
			occupe = occupe + LenTmp  ;
			cpt++ ;
			i = i + LenTmp ;
			continue ;
		}
	}
	printf("mfifo_free_memory , place restante : %d \n" , occupe );
	if ( occupe ==  0)
		return fifo->capacity ;

	//occupe = occupe + (cpt*8) ;

	return ( occupe + (cpt*8 ) ) ;
}

/**
* Cette fonction déconnect, rend inutilisable, un objet mfifo, retourn -1 en
* cas d'erreur sinon 0
*
* @param fifo	objet mfifo à rendre ne plus utiliser
* @return 		renvoie 0 en cas de succès, -1 sinon
*/
int mfifo_disconnect(mfifo *fifo){
	if( munmap(fifo->memory , fifo->capacity) == -1){
		perror("munmap ");
		return -1;
	}
	return 0;
}

/**
* Supprime un objet mfifo par son nom 
*
* @param nom 	nom de l'objet mfifo à supprimer
* @return 		retourne -1 en cas d'erreur sinon 0
*/
int mfifo_unlink(const char * nom){
	//printf("dans unlink : nom := %s \n",nom );
	int r = shm_unlink(nom);
	if( r == -1){
		perror("shm unlink");
	}
	return r;
}

/**
* Créer un objet message
*
* @param buf contenu du message à créer
*/
void create_message(char * buf, message * res){
	//printf("dans create  message\n");
	int taille = sizeof(message)+strlen(buf)+1;
	realloc(res,taille);
	//printf("malloc\n");
	res->l = strlen(buf)+1;
	//printf("len : %ld\n", res->l);
	memset(res->mes, 0, strlen(res->mes));
	memmove(res->mes,buf,res->l);
	//printf("mem : %s\n", res->mes);
	//printf("l %ld\n",strlen(res->l) );
	//printf("l : %ld + contenu : %ld = %ld\n",sizeof(res->l), res->l, sizeof(res) );
	//return res;
}