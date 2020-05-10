# Sys2020_TRAORE_CADIOU

**structures utilisées :**

typedef struct{
	const char * nom ; // le nom du mfifo , qui sera associé au shm.
	size_t capacity; // la capacité en memoire de ce mfifo
	size_t debut; // l'adresse de debut du mfifo
	size_t fin;   // l'adresse de fin du mfifo
	pid_t pid;    // le PID du processus possedant le verrou
	sem_t sem;    // la semaphore utilisé pour verrouller le mfifo pendant la lecture
	char memory[];  // un tableau de capacité [capacity] representant la memoire du mfifo.
} mfifo;

typedef struct{
	size_t l;       // la longueur du message a suivre
	char mes[];     // le contenue de ce message
} message;

**Descriptif des fonctions :**

*mfifo * mfifo_connect( const char *nom, int options, mode_t permission, size_t capacite );*

- Permet la connexion et/ou la creation d'un mfifo en memoire.
    suivant l'option choisie : 
    - 0 : demande la connexion au mfifo nommé avec le nom "nom" avec les permissions et la capacité passés en arguments.
        Dans ce cas un appel à mfifo * connexion_mfifo_nomme(char * name, size_t capacite, mode_t permission) est réalisé.
    - O_CREAT : demande la creation puis la connexion au mfifo nommé avec le nom "nom" avec les permissions et la capacité passés en arguments.
        Dans le cas ou mfifo existe deja n appel à mfifo * connexion_mfifo_nomme(char * name, size_t capacite, mode_t permission) est réalisé.
        Dans le cas ou mfifo n'existe pas cas un appel à mfifo * creation_mfifo_nomme(char * name, size_t capacite, mode_t permission) est réalisé.
    - O_CREAT|O_EXCL : demande la creation puis la connexion au mfifo nommé ( seulement s'il n'existe pas ) avec le nom "nom" avec les permissions et la capacité passés en arguments.
        Dans ce cas un appel à mfifo * creation_mfifo_nomme(char * name, size_t capacite, mode_t permission) est réalisé.
- Dans le cas ou nom est egal a NULL , un mfifo anonyme est créé , avec les permissions et la capacité souhaitées.
- Retourne : 
    l'adresse du mfifo si OK
    NULL sinon.
    
   
*mfifo * creation_mfifo_nomme(char * name, size_t capacite, mode_t permission);*

- Permet la creation d'un mfifo avec son shm associé portant le nom "nom" et les permissions et capcité souahitées.
- Retourne :
	l'adresse du mfifo si OK
	NULL sinon

*mfifo * connexion_mfifo_nomme(char * name, size_t capacite, mode_t permission);*

- Permet la connexion d'un mfifo avec son shm associé portant le nom "nom" et les permissions et capacité souahitées.
- Retourne :
	l'adresse du mfifo si OK
	NULL sinon
	
*int mfifo_disconnect(mfifo *fifo);*

- Permet de déconnecter le processus de l’objet mfifo (l’objet n’est pas détruit, mais fifo devient
inutilisable).
- Retourne 0 si OK et −1 en cas d’erreur.

*int mfifo_unlink(const char*nom);*

- Permet de supprimer l’objet mfifo nommé
- Retourne 0 si OK, −1 si échec .


*int mfifo_write(mfifo *fifo, const void *buf, size_t len);*

- Permet de bloquer le processus appelant jusqu’à ce que len octets soient écrits dans
fifo. Cette fonction est bloquant tant que les len octets n'ont pas été ecrit.

*int mfifo_trywrite(mfifo *fifo, const void *buf, size_t len);*

- mfifo_trywrite() est une version non-bloquante de mfifo_write().
- Retourne : 0 si OK et −1 en cas d’erreur
Cas sepcifique : s’il n’y a pas assez de place pour écrire len octets, mfifo_trywrite() retourne immédiatement −1 et met
errno à la valeur EAGAIN

*int mfifo_write_partial(mfifo *fifo, const void *buf, size_t len);*

*ssize_t mfifo_read(mfifo *fifo, void *buf, size_t len);*

- Permet de copier len octets de mfifo à l’adresse buf. Les octets copiés sont supprimés du mfifo et
mfifo_read.
- Retourne le nombre d’octets copiés.

int mfifo_lock(mfifo *fifo);

- Permet de bloquer jusqu’à ce que le processus appelant obtienne le verrou.

int mfifo_unlock(mfifo*fifo);

- Permet de debloquer le verrou associé au mfifo.

int mfifo_trylock(mfifo*fifo);

- Permet d'essayer de prendre le verrou 
- Retourne 0 si OK ,
	-1 et errno = EAGAIN sinon.

size_t mfifo_capacity(mfifo*fifo);

- Retourne la capacité du mfifo.

size_t mfifo_free_memory(mfifo*fifo);

- Retourne le quantité de places disponibles dans le mfifo.

**Fonctions annexes :**

*void fill_mfifo(mfifo * fifo, size_t addr, size_t capacite, char *name);*

- Permet de remplir la structure mfifo avec les informations données.

*void init_memory_mfifo(mfifo * fifo);*

- Permet d'initialiser la memoire d'un mfifo a 0 sur capacity octets.

*int free_mfifo(mfifo *fifo);*

- Permet de liberer l'espace memoire associé au mfifo.

*void check_return_errno();*

- Permet un affichage plus clair des errno.

*void print_fifo_memory(mfifo * fifo );*

- Permet d'afficher le contenu en memoire du mfifo ( mfifo->memory ).

*void create_message(char * buf, message * res);*

- Permet de creer une structure message contenant le contenue de buf.


