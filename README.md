# Sys2020_TRAORE_CADIOU

## Description du Projet

Dans le cadre du cours de Système avancé de ce semestre, nous avons réaliser le projet répondant au sujet : [Enoncé du Projet](https://moodlesupd.script.univ-paris-diderot.fr/pluginfile.php/1100882/mod_resource/content/3/projet2020_ver3.pdf) 

// NEED TO FILL THE BLANK WALLAH

Le répertoire git contient deux dossiers : **src/** et **test/** .
Le dossier src contient le code correspondant à la **bibliothèque mfifo**  devant être produite pour répondre au sujet du projet. D'un autre côté, le dossier test contient les testes unitaires liés aux fonctions de la bibliothèque mfifo.  Au total le repertoire Git contient deux fichiers Makefile, un pour pour chacun des deux dossiers évoqués précédement.

// NEED TO FILL THE BLANK AGAIN 

Pour lancer la série de testes unitaires, il suffit d'utiliser les commandes :
```bash
	make
	make test
```
Ensuite vérifier que 100%  des testes effectués se sont bien dérouler.
Les testes concernent uniquement les fonctions de la **bibliothèque mfifo** demandé dans le sujet, vérifiant ainsi certains cas où les fonctionnalités sont censées échouer mais aussi les cas valide d'utilisation.


## Description du code (Manuel):

### Les structures utilisées :
```bash
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
```

La structure message ne fait pas partie à part entière de la bibliothèque mfifo, elle n'a été implémentée que pour tester le comportement de certaines fonctions (liées à l'écriture et la lecture principalement) afin de nous indiquer comment se rapprocher au mieux des consignes du sujet.

Dans la structure **mfifo**, nous avons eu l'opportunité entre diverse choix pour gérer **l'exclusion mutuelle** et nous avons décider d'implémenter cette partie en utilisant les sémaphores pour gérer les sections critiques du projet.

### Le descriptif des fonctions :

#### Fonctions Principales

-----------------------------------------------------------------------------------------------

**NAME**

mfifo_connect

**SYNOPSIS**

mfifo * mfifo_connect( const char * nom , int options , mode_t permission , size_t capacite ;

**DESCRIPTION**

La fonction **mfifo_connect**() permet la connexion et/ou la creation d'un mfifo en memoire, suivant l'option choisie. 

 - **0** :  demande la connexion au mfifo nommé avec le nom "nom" avec les permissions et la capacité passés en arguments.
Dans ce cas un appel à mfifo * connexion_mfifo_nomme(char * name, size_t capacite, mode_t permission) est réalisé.

- **O_CREAT** : demande la creation puis la connexion au mfifo nommé avec le nom "nom" avec les permissions et la capacité passés en arguments.
        Dans le cas ou mfifo existe deja n appel à mfifo * connexion_mfifo_nomme(char * name, size_t capacite, mode_t permission) est réalisé.
        Dans le cas ou mfifo n'existe pas cas un appel à mfifo * creation_mfifo_nomme(char * name, size_t capacite, mode_t permission) est réalisé.
        
- **O_CREAT|O_EXCL** : demande la creation puis la connexion au mfifo nommé ( seulement s'il n'existe pas ) avec le nom "nom" avec les permissions et la capacité passés en arguments.
        Dans ce cas un appel à mfifo * creation_mfifo_nomme(char * name, size_t capacite, mode_t permission) est réalisé.
        
Dans le cas ou nom est egal a NULL , un mfifo anonyme est créé , avec les permissions et la capacité souhaitées.

**RETURN**

Renvoie l'adresse du mfifo si OK,NULL sinon.
   -----------------------------------------------------------------------------------------------
   
**mfifo * creation_mfifo_nomme(char * name, size_t capacite, mode_t permission);**

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

-----------------------------------------------------------------------------------------------

**int mfifo_write(mfifo *fifo, const void *buf, size_t len);**

- Permet de bloquer le processus appelant jusqu’à ce que len octets soient écrits dans
fifo. Cette fonction est bloquant tant que les len octets n'ont pas été ecrit.

*int mfifo_trywrite(mfifo *fifo, const void *buf, size_t len);*

- mfifo_trywrite() est une version non-bloquante de mfifo_write().
- Retourne : 0 si OK et −1 en cas d’erreur
Cas sepcifique : s’il n’y a pas assez de place pour écrire len octets, mfifo_trywrite() retourne immédiatement −1 et met
errno à la valeur EAGAIN

-----------------------------------------------------------------------------------------------

**int mfifo_write_partial(mfifo *fifo, const void *buf, size_t len);**
// NEED TO FILL THE BLANK WALLLAAAAAHRRHRHRH

-----------------------------------------------------------------------------------------------

**ssize_t mfifo_read(mfifo *fifo, void *buf, size_t len);**

- Permet de copier len octets de mfifo à l’adresse buf. Les octets copiés sont supprimés du mfifo et
mfifo_read.
- Retourne le nombre d’octets copiés.

-----------------------------------------------------------------------------------------------

**int mfifo_lock(mfifo *fifo);**

- Permet de bloquer jusqu’à ce que le processus appelant obtienne le verrou.

-----------------------------------------------------------------------------------------------

**int mfifo_unlock(mfifo*fifo);**

- Permet de debloquer le verrou associé au mfifo.

-----------------------------------------------------------------------------------------------

**int mfifo_trylock(mfifo*fifo);**

- Permet d'essayer de prendre le verrou 
- Retourne 0 si OK ,
	-1 et errno = EAGAIN sinon.

size_t mfifo_capacity(mfifo*fifo);

- Retourne la capacité du mfifo.

-----------------------------------------------------------------------------------------------

**size_t mfifo_free_memory(mfifo*fifo);**

- Retourne le quantité de places disponibles dans le mfifo.

-----------------------------------------------------------------------------------------------

#### Fonctions Annexes :

**void fill_mfifo(mfifo * fifo, size_t addr, size_t capacite, char *name);**

- Permet de remplir la structure mfifo avec les informations données.

**void init_memory_mfifo(mfifo * fifo);**

- Permet d'initialiser la memoire d'un mfifo a 0 sur capacity octets.

**int free_mfifo(mfifo *fifo);**

- Permet de liberer l'espace memoire associé au mfifo.

**void check_return_errno();**

- Permet un affichage plus clair des errno.

**void print_fifo_memory(mfifo * fifo );**

- Permet d'afficher le contenu en memoire du mfifo ( mfifo->memory ).

**void create_message(char * buf, message * res);**

- Permet de creer une structure message contenant le contenue de buf.


