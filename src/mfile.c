
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


#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _b : _a; })


static void fill_mfifo(mfifo * fifo, size_t addr, size_t capacite, char *name);
static mfifo * connexion_mfifo_nomme(char * name, size_t capacite, mode_t permission);
static mfifo * creation_mfifo_nomme(char * name, size_t capacite, mode_t permission);
static void init_memory_mfifo(mfifo * fifo);



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
		mfifo * res = malloc(sizeof(mfifo)+capacite);
		res = mmap(NULL, capacite, PROT_READ | PROT_WRITE,  MAP_SHARED | MAP_ANONYMOUS, -1, 0);
		if( res == MAP_FAILED){
			//mfifo_unlink(name);
			perror("O - mmap");
			exit(1);
		}
		printf("Creation fifo anonyme | adresse du fifo : %p\n",  res );
		fill_mfifo(res,(size_t) res , capacite, NULL);
		//printf("Creation anonyme : capa = %d \n", res->capacity );
	    return res ;
	}
	if ( nom != NULL ){
		char * name = malloc(sizeof(char)*(sizeof(nom)+2));
		strcat(name, "/");
		strcat(name, nom);
		switch(options){
			case 0 :
				//printf("\nCONNEXION ....\n");
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
							//printf("echec de connexion\n");
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
		return NULL;
	}
	struct stat buf_stat;
	if (fstat(fd, &buf_stat) == -1) {
    	perror(" fstat ");
    	return NULL;
    }
	res = mmap(NULL, capacite, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	if( res == MAP_FAILED){
		//mfifo_unlink(name);
		perror("O - mmap");
		return NULL;
	}
    close(fd);
	printf("Connexion Fifo '%s'| adresse du fifo : %p\n", name , res );
	fill_mfifo(res,(size_t) res , capacite, name);
   	return res;
}

mfifo * creation_mfifo_nomme(char * name, size_t capacite, mode_t permission){
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
    	return NULL;
	}
	if( buf_stat.st_size == 0 && ftruncate( fd, capacite ) == -1){
		perror("Ftruncate");
		return NULL;
	}
  	res = mmap(NULL, capacite, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	if( res == MAP_FAILED){
	    perror("O_CREAT - mmap");
	    return NULL;
  	}
	close(fd);
	printf("Creation Fifo | creation d'un mfifo_Vide , adresse du fifo : %p\n", &res[0] );
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
static void fill_mfifo(mfifo * fifo, size_t addr, size_t capacite, char *name){
	
	if(name == NULL){
		fifo->nom = NULL;
	}else{
		fifo->nom = malloc(strlen(name));
		memcpy((char*)fifo->nom, name, strlen(name));
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
	memset(fifo->memory, 0, fifo->capacity);
}

/**
* Permet un ecriture partiel de message/
*
* @param : l'objet fifo dans lequel on veut ecrire , la valeur de ce qu'on veut ecrire , 
		   la taille de cette derniere
*
*/
int mfifo_write_partial(mfifo *fifo, const void *val, size_t len){
	//printf("Buf : %s | Len = %d \n", val , len  );
	if ( (int)len < 0 ){
		printf("On quitte car Len negative.\n");
		return -1 ;
	}
	else if ( (int)len == 0 ){
		printf("On quitte car on a ecrit tout le contenue de Val.\n");
		return 0 ;
	}
	int dispo  = mfifo_free_memory(fifo) ;
	//printf("A l'entrée de la boucle , on a Dispo = %d \n",  dispo);
	while ( dispo <= 0 ){
		dispo  = mfifo_free_memory(fifo) ;
	}

	//printf("On quitte la boucle Dispo car Dispo = %d \n", dispo );
	char * tmp = malloc(dispo+1) ;
	memcpy(tmp,val,dispo);
	printf("Buf a ecrire sur '%ld' octets : %s\n", len , tmp );

	int index = fifo->capacity - dispo ;

	memcpy(&fifo->memory[index] , tmp , dispo) ;

	val = val+dispo ;

	printf("Reste a ecrire : %s \n", (char*)val );

	if ( sizeof(val) != 0 ){
		if ( (int)(len - dispo )< 0){
			len = 0 ;
		}
		else {
			len = len - dispo ;
		}
		mfifo_write_partial(fifo,val,len) ;
	}
	return 0;
}

/*
* Permet l'affichage du ERRNO avec un message associé.
*/
void check_return_errno(){
	if ( errno == EAGAIN ){
			perror("L'ecriture a echouée (EAGAIN)");
		}
	if (errno == EMSGSIZE ){
			perror("L'ecriture a echouée (EMSGSIZE) ");
		}
}

/*
* Permet une tentative d'ecriture dans le fifo non  bloquante .
*
*@param : l'objet fifo dans lequel on veut ecrire , la valeur de ce qu'on veut ecrire , 
		   la taille de cette derniere
*
* @return : 0 si OK , -1 sinon , avec errno adapté.
*/
int mfifo_trywrite(mfifo *fifo, const void *val, size_t len){
	if ( val == NULL || len <= 0 ){
		perror("Invalid arguments ( val or len )") ;
		return -1;
	}
	// On test que LEN est bien < fifo->capacite .
	if ( len > fifo->capacity ){
		errno = EMSGSIZE;
		return -1;
	}
	
	int dispo  = mfifo_free_memory(fifo) ;
	if ( (int)len > dispo ){
		printf("try_Write | Pas assez de place disponible pour ecrire dans le fifo ( len : %ld , dispo : %d )\n" , len , dispo );
		dispo = mfifo_free_memory(fifo) ;
		errno = EAGAIN ;
		return -1 ;
	}
	int index = fifo->capacity - dispo ;
	memcpy(&fifo->memory[index] , val , len) ;
	printf("Try_Write | Dans le fifo '%s' , on ajoute a l'index [%d] : '", fifo->nom , index);
	for ( int i = index ; i < (int)(index+len) ; i++ ){
		printf("%c", fifo->memory[i]);
	}
	printf("'\n");
	if ( fifo->nom == NULL ){
		msync(fifo, fifo->capacity, MS_SYNC); 
		//printf("Synchronisation memoire try_write : %d \n" , resu );
	}
	return 0;
}


/*
* Permet une tentative d'ecriture dans le fifo bloquante .

* Bloque le processus tant que l'ecriture n'a pas eu lieu.
*
*@param : l'objet fifo dans lequel on veut ecrire , la valeur de ce qu'on veut ecrire , 
		   la taille de cette derniere
*
* @return : 0 si OK , -1 sinon , avec errno adapté.
*/
	
int mfifo_write(mfifo *fifo, const void *val, size_t len){
	// On test que LEN est bien < fifo->capacite .
	if ( val == NULL || (int)len <= 0 ){
		perror("Invalid arguments ( val or len )") ;
		return -1;
	}
	if ( len > fifo->capacity ){
		perror("Write | Erreur , Len > fifo->capacite\n");
		printf("Write | Len : %ld, fifo->capacity : %ld, cpt : %ld \n",len,fifo->capacity, mfifo_free_memory(fifo) );
		errno = EMSGSIZE;
		return -1;
	}

	int dispo  = mfifo_free_memory(fifo) ;
	while ( (int)len > dispo ){
		dispo = mfifo_free_memory(fifo) ;
	}
	int index = fifo->capacity - dispo ;
	memcpy(&fifo->memory[index] , val , len) ;
	//printf("Val : %s , len %d , dispo %d \n", val , len , dispo );
	printf("Write | Dans le fifo '%s' , on ajoute a l'index [%d] : '", fifo->nom , index);
	for ( int i = index ; i < (int)(index+len) ; i++ ){
		printf("%c", fifo->memory[i]);
	}
	printf("'\n");

	if ( fifo->nom == NULL ){
		msync(fifo, fifo->capacity, MS_SYNC); 
		//printf("Synchronisation memoire write : %d \n" , resu );
	}
	
	return 0;
}

/*
Permet la lecture par message .
*/ 
size_t mfifo_read_message(mfifo *fifo, void *buf ) {
	// on bloque le fifo
	if ( mfifo_lock(fifo) == 0 ) {
		fifo->pid = getpid() ;
		printf("Le verrou de lecture est detenu par le processus n° %d \n", fifo->pid );
	} 

	message *ps = malloc( sizeof( message) );

	mfifo_read(fifo, ps, sizeof(message)); 

	ps = realloc( ps, ps->l + sizeof(message) ); 

	ps->l = mfifo_read(fifo, ps , ps->l ); 

	memcpy(buf,ps->mes,ps->l) ;

	if ( mfifo_unlock(fifo) == 0 ) // on debloque
	{
		printf("Le processus n°%d lache le verrou.\n" , fifo->pid);
		fifo->pid = -1 ;
	}
	return strlen(buf) ;

}

/*
*	Affiche le contenue de la memoire du fifo passé en argument.
*
*/
void print_fifo_memory(mfifo * fifo ){
	for ( int i = 0 ; i < (int)fifo->capacity ; i++ ){
		int jmp = strlen(&fifo->memory[i]) ;
		printf("%s", &fifo->memory[i] );
		i += jmp ;
	}
	printf("\n");
}

/* Permet de placer le contenu de fifo->memory dans buf , sur len octets. 
	Vide ensuite la memoire du fifo de len octets en aprtant du debut.
*
*	@param :  le fifo concerné , le buffer dans lequel on veut placer le contenue , 
	la taille de lecture souhaité.

	@return : len si OK , -1 si fifo inaccessible , 0 si rien a lire ( fifo vide ).
*
*/
ssize_t mfifo_read(mfifo *fifo, void *buf, size_t len){

	if ( (int)len <= 0 ){
		perror("Invalid arguments ( len )") ;
		return -1;
	}
	if ( mfifo_lock(fifo) == -1 ) {
		printf("Verrou inaccessible.\n");
		return -1 ;
	} 
	if ( fifo->nom == NULL ){
		msync(fifo, fifo->capacity, MS_ASYNC); 
		//printf("Synchronisation memoire read : %d \n" , resu );
	}
	size_t dispo  = mfifo_free_memory(fifo) ;

	if ( dispo == fifo->capacity){
		printf("Read | Le fifo est vide. Rien a lire .\n");
		if ( mfifo_unlock(fifo) == 0 ) // on debloque
		{
			printf("Le processus n°%d lache le verrou.\n" , fifo->pid);
			fifo->pid = -1 ;
		}
		return 0 ;
	}
	memcpy(buf , &fifo->memory[0] , len) ;
	memmove(&fifo->memory[0] , &fifo->memory[len] , fifo->capacity ) ;

	printf("PID : %d , Read : '%s' \n", getpid() , (char*)buf );

	if ( mfifo_unlock(fifo) == 0 ) // on debloque
	{
		fifo->pid = -1 ;
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
	if ( fifo == NULL ){
		return -1 ;
	}
	/*ajout du pid du processus appelant dans la queue de la semaphore*/
	if(sem_wait(&fifo->sem) == -1){
		perror("sem wait ");
		return -1;
	}
	fifo->pid = getpid() ;
	printf("\nLe verrou de lecture est detenu par le processus n° %d\n", fifo->pid );
	return 0;
}

/**
*	Essais de vérouiller l'objet mfifo
*
* @param 	fifo objet mfifo sur lequel nous voulons essayer de poser un verrou
* @return 	revoie 0 si l'obtention du verrou réussi, -1 sinon
*/
int mfifo_trylock(mfifo *fifo){
	if ( fifo == NULL ){
		return -1 ;
	}
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
	if ( fifo == NULL ){
		return -1 ;
	}
	if(sem_post(&fifo->sem) == -1 ){
		perror("sem post  ");
		return -1;
	}
	printf("Le processus n°%d lache le verrou.\n" , fifo->pid);
	return 0;
}

/*
* Informe de la capacité totale de l'objet mfifo
*
* @param fifo 	objet mfifo dont l'on savoir la capacité de stockage
* @return 		renvoie la capicité totale de stockage de fifo
*/
size_t mfifo_capacity(mfifo *fifo){
	if ( fifo == NULL ){
		return -1 ;
	}
	return fifo->capacity;
}


/**
* Renvoie la quantié de mémoire libre de l'objet mfifo
*
* @param fifo 	objet mfifo dont l'on veut savoir la quantité de mémoire libre
* @return 		renvoie la quantité de mmémoire libre de fifo
*/
size_t mfifo_free_memory(mfifo *fifo){
	if ( fifo == NULL ){
		return -1 ;
	}
	/* TODO */
	int index = 0 ;
	for ( int i = fifo->capacity ; i > 0 ; i -- ){
		if ( fifo->memory[i] != '\0'){
			index = i+1 ;
			break ;
		}

	}
	int res = fifo->capacity - index ;
	return res ;
}

/**
* Cette fonction déconnect, rend inutilisable, un objet mfifo, retourn -1 en
* cas d'erreur sinon 0
*
* @param fifo	objet mfifo à rendre ne plus utiliser
* @return 		renvoie 0 en cas de succès, -1 sinon
*/
int mfifo_disconnect(mfifo *fifo){
	if ( fifo == NULL ){
		return -1 ;
	}
	if( munmap( fifo , fifo->capacity ) == -1){
		perror("munmap ");
		return -1;
	}
	printf("Deconnexion du fifo a l'adresse : %p \n", fifo );
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
	printf("Unlink du fifo : %s \n", nom );
	return r;
}

/**
* Créer un objet message
*
* @param buf contenu du message à créer
*/
void create_message(char * buf, message * res){
	int taille = sizeof(message)+strlen(buf)+1;
	res = realloc(res,taille);
	res->l = strlen(buf)+1;
	memset(res->mes, 0, strlen(res->mes));
	memmove(res->mes,buf,res->l);
}