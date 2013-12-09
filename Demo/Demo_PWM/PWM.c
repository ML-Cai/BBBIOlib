
#include <stdio.h>
#include <stdlib.h>
#include "../../BBBio_lib/BBBiolib.h"

int
main(void)
{
	iolib_init();

        BBBIO_PWMSS_Setting(0 , 5000.0f ,25.0 , 50.0);
	BBBIO_PWMSS_Enable(0);

	sleep(10);
	BBBIO_PWMSS_Disable(0);

	iolib_free();
	return(0);
}


