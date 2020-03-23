
#include <stdio.h>
#include <stdlib.h>
#include "mfile.h"

int main(void)
{
	Init();
	printf("Creation : \n");
	mfifo * a = mfifo_connect("test",O_CREAT,0777,12);

	printf("\n\nConnexion : \n");
	a = mfifo_connect("test",0,0777,12);


	printf("\n\nCreation - connexion : \n");
	mfifo * b = mfifo_connect("testBis",O_CREAT|O_EXCL,0777,12);

	


	return EXIT_SUCCESS;
}
					
