
#include <stdio.h>
#include <stdlib.h>
#include "mfile.h"

#define LEN 512

int main(void)
{
	Init();
	printf("Creation : \n");
	mfifo * a = mfifo_connect("test",O_CREAT,0777,LEN);

	printf("\n\nConnexion : \n");
	a = mfifo_connect("test",0,0777,LEN);


	printf("\n\nCreation - connexion : \n");
	mfifo * b = mfifo_connect("testBis",O_CREAT|O_EXCL,0777,LEN);

	


	return EXIT_SUCCESS;
}
					
