# Sys2020_TRAORE_CADIOU

## Description du Projet

Dans le cadre du cours de Système avancé de ce semestre, nous avons réaliser le projet répondant au sujet : [Enoncé du Projet](https://moodlesupd.script.univ-paris-diderot.fr/pluginfile.php/1100882/mod_resource/content/3/projet2020_ver3.pdf) 

Le but du projet est d’implémenter un système de communication inter-processus qui permet deux types de communications :
- une communication similaire à celle par tube (pipe ou fifo),
- et une communication par messages.
Les données seront traitées selon l’algorithme fifo : les octets sont lus dans un mfifo dans l’ordre d’écriture, et la lecture supprime automatiquement les octets lus.

Le répertoire Git de ce projet contient deux dossiers : **src/** et **test/** .
Le dossier src contient le code correspondant à la **bibliothèque mfifo**  devant être produite pour répondre au sujet du projet. D'un autre côté, le dossier test contient les testes unitaires liés aux fonctions de la bibliothèque mfifo.  Au total le repertoire Git contient deux fichiers Makefile, un pour pour chacun des deux dossiers évoqués précédement.

Nous avons écrit un programme principal qui permet de visualiser l'ensemble des fonctionnalitées du projet.
Pour le lancer il faut se placer dans **/src** et ensuite entrer les commandes suivantes :
```bash
	make
	./mfile
```
Ce main reprend point par point les fonctions d'écriture et de lecture entre divers processus, nous avons essayé de produire un affichage permettant de séparer les parties.
En resumé :

```bash
Partie 1 : Ecriture / Lecture dans un mfifo anonyme entre 2 processus.
		( la structure Message a été utilisé ici )
		
Partie 2 : Ecriture / Lecture dans un mfifo nommé entre 2 processus.
		( Un buffer été utilisé utlisé ici )
		
Partie 3 : Une boucle d'écriture ( répété 50 fois ) ainsi qu'une lecture effectué par deux processus differents ( une boucle de lecture pour chaque processus ), de maniere à mettre en évidence l'usage de nos verrous , et le fait que l'écriture ne gène en rien la lecture.
		
Partie 4 : Essaie d'écriture ( avec Try_write ) , suivie de plusieurs lectures pour mettre en évidence le fonctionnement de Trywrite().

Partie 5 : Essaie d'écriture ( avec Write_partial) , suivie de plusieurs lectures pour mettre en évidence le fonctionnement de Write_partial().

```


Pour lancer la série de testes unitaires, se placer dans **test/** et ensuite il suffit d'utiliser les commandes:
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

Dans la structure **mfifo**, nous avons eu l'opportunité de trancher entre diverse choix pour gérer **l'exclusion mutuelle** et nous avons décider d'implémenter cette partie en utilisant les sémaphores pour gérer les sections critiques du projet.

### Le descriptif des fonctions :

#### Fonctions Principales

-----------------------------------------------------------------------------------------------

**NAME**

mfifo_connect

**SYNOPSIS**

mfifo * mfifo_connect( const char * nom , int options , mode_t permission , size_t capacite ;

**DESCRIPTION**

La fonction **mfifo_connect**() permet la connexion et/ou la creation d'un mfifo en memoire, suivant l'option choisie. 

 - **0** :  demande la connexion au mfifo nommé avec le nom **nom** avec les **permissions** et la **capacité** passés en arguments.
Dans ce cas un appel à **connexion_mfifo_nomme**() est réalisé.

- **O_CREAT** : demande la création puis la connexion au mfifo nommé avec le nom **nom** avec les **permissions** et la **capacité** passés en arguments.
Dans le cas ou mfifo existe déjà un appel à **connexion_mfifo_nomme**() est réalisé.
Dans le cas ou mfifo n'existe pas cas un appel à **creation_mfifo_nomme**() est réalisé.
        
- **O_CREAT|O_EXCL** : demande la creation puis la connexion au mfifo nommé, seulement s'il n'existe pas, avec le nom **nom** avec les **permissions** et la **capacité** passés en arguments.
Dans ce cas un appel à **creation_mfifo_nomme**() est réalisé.
        
Dans le cas ou nom est égal a **NULL** , un mfifo anonyme est créé , avec les permissions et la capacité souhaitées.

**RETURN**

Renvoie l'adresse du mfifo si OK, NULL sinon.
   
-----------------------------------------------------------------------------------------------

-----------------------------------------------------------------------------------------------

**NAME**

mfifo_disconnect

**SYNOPSIS**

int mfifo_disconnect(mfifo *fifo);

**DESCRIPTION**

Permet de déconnecter le processus de l’objet mfifo (l’objet n’est pas détruit, mais fifo devient inutilisable).

**RETURN**

Retourne 0 si OK et −1 en cas d’erreur.

-----------------------------------------------------------------------------------------------

-----------------------------------------------------------------------------------------------

**NAME**

mfifo_unlink

**SYNOPSIS**

int mfifo_unlink(const char*nom);

**DESCRIPTION**

Permet de supprimer l’objet mfifo nommé

**RETURN**

Retourne 0 si OK, −1 si échec .

-----------------------------------------------------------------------------------------------

-----------------------------------------------------------------------------------------------

**NAME**

mfifo_write

**SYNOPSIS**

int mfifo_write(mfifo *fifo, const void *buf, size_t len);

**DESCRIPTION**

Permet d'écrire les **len** premiers octets du contentu d'un message dans la mémoire d'un mfifo. Cette fonction est bloquante tant que les **len** octets n'ont pas fini d'être écrit. 

**RETURN**

Retourne 0 si OK, −1 si échec .

-----------------------------------------------------------------------------------------------

-----------------------------------------------------------------------------------------------

**NAME**

mfifo_trywrite

**SYNOPSIS**

int mfifo_trywrite(mfifo *fifo, const void *buf, size_t len);

**DESCRIPTION**

**mfifo_trywrite**() est une version non-bloquante de mfifo_write().

**RETURN**

Retourne 0 si OK et −1 en cas d’erreur
Cas sepcifique : s’il n’y a pas assez de place pour écrire len octets, mfifo_trywrite() retourne immédiatement −1 et met errno à la valeur EAGAIN

-----------------------------------------------------------------------------------------------

-----------------------------------------------------------------------------------------------

**NAME**

mfifo_write_partial

**SYNOPSIS**

int mfifo_write_partial(mfifo *fifo, const void *buf, size_t len);

**DESCRIPTION**

Permet d'écrire **len** octets dans l'objet mfifo mais pas forcément de façon continue.
**mfifo_write_partial**() écrit le nombre d'octets maximum possible dans le fifo jusqu'à atteindre la disponibilité maximal.
Si un seul appel à **mfifo_write_partial**() ne suffit pas à écrire l'ensemble des octets voulus, d'autres appels à **mfifo_write_partial**() seront effectués jusqu'à avoir écrit l'entièreté du message souhaité.

**RETURN**

Retourne 0 si OK, −1 si échec .

-----------------------------------------------------------------------------------------------

-----------------------------------------------------------------------------------------------

**NAME**

mfifo_read

**SYNOPSIS**

ssize_t mfifo_read(mfifo *fifo, void *buf, size_t len);

**DESCRIPTION**

Permet de copier len octets de mfifo à l’adresse buf. Les octets copiés sont supprimés du mfifo et
mfifo_read.

**RETURN**

Retourne le nombre d’octets copiés.

-----------------------------------------------------------------------------------------------

-----------------------------------------------------------------------------------------------

**NAME**

mfifo_lock

**SYNOPSIS**

int mfifo_lock(mfifo *fifo);

**DESCRIPTION**

Permet de bloquer jusqu’à ce que le processus appelant obtienne le verrou.

-----------------------------------------------------------------------------------------------

-----------------------------------------------------------------------------------------------

**NAME**

mfifo_unlock

**SYNOPSIS**

int mfifo_unlock(mfifo*fifo);

**DESCRIPTION**

Permet de debloquer le verrou associé au mfifo.

-----------------------------------------------------------------------------------------------

-----------------------------------------------------------------------------------------------

**NAME**

mfifo_trylock

**SYNOPSIS**

int mfifo_trylock(mfifo*fifo);

**DESCRIPTION**

Permet d'essayer de prendre le verrou 

**RETURN**

Retourne 0 si OK , -1 et errno = EAGAIN sinon.

-----------------------------------------------------------------------------------------------

-----------------------------------------------------------------------------------------------

**NAME**

mfifo_capacity

**SYNOPSIS**

size_t mfifo_capacity(mfifo*fifo);

**RETURN**

Retourne la capacité du mfifo.

-----------------------------------------------------------------------------------------------

-----------------------------------------------------------------------------------------------

**NAME**

mfifo_free_memory

**SYNOPSIS**

size_t mfifo_free_memory(mfifo*fifo);

**RETURN**

Retourne le quantité de places disponibles dans le mfifo.

-----------------------------------------------------------------------------------------------

-----------------------------------------------------------------------------------------------

#### Fonctions Annexes :


**mfifo * creation_mfifo_nomme(char * name, size_t capacite, mode_t permission);**

- Permet la creation d'un mfifo avec son shm associé portant le nom "nom" et les permissions et capcité souahitées.
- Retourne :
	l'adresse du mfifo si OK
	NULL sinon

**mfifo * connexion_mfifo_nomme(char * name, size_t capacite, mode_t permission);**

- Permet la connexion d'un mfifo avec sa Shared Memory associée portant le nom **nom** et les permissions et capacité souahitées.
- Retourne :
	l'adresse du mfifo si OK
	NULL sinon

**void fill_mfifo(mfifo * fifo, size_t addr, size_t capacite, char *name);**

- Permet de remplir la structure mfifo avec les informations données.

**void init_memory_mfifo(mfifo * fifo);**

- Permet d'initialiser la memoire d'un mfifo à 0 sur **fifo->capacity** octets.

**int free_mfifo(mfifo *fifo);**

- Permet de libérer l'espace mémoire associé au mfifo.

**void check_return_errno();**

- Permet un affichage plus clair des errno.

**void print_fifo_memory(mfifo * fifo );**

- Permet d'afficher le contenu en mémoire du mfifo ( mfifo->memory ).

**void create_message(char * buf, message * res);**

- Permet de créer une structure message contenant le contenue de buf.


