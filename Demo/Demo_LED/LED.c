
#include <stdio.h>
#include <stdlib.h>
#include "../../BBBio_lib/BBBiolib.h"

int
main(void)
{
	int del;
	iolib_init();
	iolib_setdir(8,11, BBBIO_DIR_IN);
	iolib_setdir(8,12, BBBIO_DIR_OUT);

	int count = 0;
	while(count < 50)
	{
		count ++ ;
		if (is_high(8,11))
		{
			del=100; // fast speed
		}
		if (is_low(8,11))
		{
			del=500; // slow speed
		}

		pin_high(8,12);
		iolib_delay_ms(del);
		pin_low(8,12);
		iolib_delay_ms(del);

	}
	iolib_free();
	return(0);
}


