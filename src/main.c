
#include <stdio.h>
#include <stdlib.h>
#include "mfile.h"

int main(void)
{
	Init();
	mfifo * a = mfifo_connect("test",0,O_CREAT,12);
	return EXIT_SUCCESS;
}
					
