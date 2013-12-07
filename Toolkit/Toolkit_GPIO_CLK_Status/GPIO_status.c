
#include <stdio.h>
#include <stdlib.h>
#include "../../BBBio_lib/BBBiolib.h"

int
main(void)
{
	iolib_init();

	BBBIO_sys_GPIO_CLK_status();
	iolib_free();
	return(0);
}


