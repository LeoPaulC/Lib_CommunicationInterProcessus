#ifndef H_MFILE
#define H_MFILE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/stat.h>

/* mfifo type : */

typedef struct{
	const char * nom ;
	size_t capacity;
	size_t debut;
	size_t fin;
	pid_t pid;
	sem_t sem;
	/* semaphores , mutexes, conditions */
	char memory[];
} mfifo;

typedef struct{
	size_t l;
	char mes[];
} message;


/* Associated fonctions  */

void Init(void);
mfifo * mfifo_connect( const char *nom, int options, mode_t permission, size_t capacite );
int mfifo_disconnect(mfifo *fifo);
int mfifo_unlink(const char*nom);

int mfifo_write(mfifo *fifo, const void *buf, size_t len);
int mfifo_trywrite(mfifo *fifo, const void *buf, size_t len);
int mfifo_write_partial(mfifo *fifo, const void *buf, size_t len);

ssize_t mfifo_read(mfifo *fifo, void *buf, size_t len);

int mfifo_lock(mfifo *fifo);
int mfifo_unlock(mfifo *fifo);
int mfifo_trylock(mfifo *fifo);

size_t mfifo_capacity(mfifo *fifo);
size_t mfifo_free_memory(mfifo *fifo);

static void fill_mfifo(mfifo * fifo, size_t addr, size_t capacite, char *name);
static void init_memory_mfifo(mfifo * fifo);
int free_mfifo(mfifo *fifo);
void check_return_errno();
void print_fifo_memory(mfifo * fifo );

static void create_message(char * buf, message * res);
static mfifo * connexion_mfifo_nomme(char * name, size_t capacite, mode_t permission);
static mfifo * creation_mfifo_nomme(char * name, size_t capacite, mode_t permission);

#endif
					

