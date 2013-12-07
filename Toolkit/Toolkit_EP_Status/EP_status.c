
#include <stdio.h>
#include <stdlib.h>
#include "../../BBBio_lib/BBBiolib.h"

int
main(void)
{
	iolib_init();

	BBBIO_sys_Expansion_Header_status(8);
        BBBIO_sys_Expansion_Header_status(9);

	iolib_free();
	return(0);
}


