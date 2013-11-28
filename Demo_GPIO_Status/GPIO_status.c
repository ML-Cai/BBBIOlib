
#include <stdio.h>
#include <stdlib.h>
#include "../BBBio_lib/BBBiolib.h"

int
main(void)
{
	int del;
	iolib_init();

	BBBIO_sys_Enable_GPIO(BBBIO_GPIO2);
	BBBIO_sys_GPIO_CLK_status();
	BBBIO_sys_Expansion_Header_status(8);
        BBBIO_sys_Expansion_Header_status(9);


	iolib_free();
	return(0);
}


