
#include <stdio.h>
#include <stdlib.h>
#include "../../BBBio_lib/BBBiolib.h"
/* -------------------------------------------------------------- */
int main(void)
{
	iolib_init();

	BBBIO_PWMSS_Setting(BBBIO_PWMSS0 , 1000000.0f ,50.0 , 50.0);
	printf("PWM %d setting\n", BBBIO_PWMSS0);
	sleep(5);
	printf("PWM %d enable\n", BBBIO_PWMSS0);
	BBBIO_ehrPWM_Enable(BBBIO_PWMSS0);
	sleep(5);
	printf("PWM %d disable\n", BBBIO_PWMSS0);
	BBBIO_ehrPWM_Disable(BBBIO_PWMSS0);
	iolib_free();
	return(0);
}


