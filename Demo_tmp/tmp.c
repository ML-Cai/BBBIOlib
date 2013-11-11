
#include <stdio.h>
#include <stdlib.h>
#include "../BBBio_lib/BBBiolib.h"
//-----------------------------------------------------------------
int
main(void)
{
	int del =5;
	iolib_init();

	iolib_setdir(9,14, BBBIO_DIR_OUT);

	int count = 0;
	while(count < 10)
	{
		if (count% 2==0)
			pin_high(9,14);
		else
			pin_low(9,14);

		if (is_high(9,14))
			printf ("8-40 is HIGH\n");

		if (is_low(9,14))
			printf("8-40 is LOW\n");

		sleep(1);

		count ++ ;
	}
	iolib_free();
	return(0);
}


