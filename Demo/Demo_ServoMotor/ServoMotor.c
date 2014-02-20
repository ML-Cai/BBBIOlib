
#include <stdio.h>
#include <stdlib.h>
#include "../../BBBio_lib/BBBiolib.h"
/* ----------------------------------------------------------- */
int main(void)
{
	iolib_init();
	int i;
	float SM_1_duty ;	/* Sermotor 1 , connect to ePWM0A */

	for(i = 0 ; i <= 180 ; i += 10)
	{
	    SM_1_duty  = 4.0f + i * 0.04444f ;
	    printf("Angle : %d , duty : %f\n" ,i ,SM_1_duty);
	    BBBIO_PWMSS_Setting(BBBIO_PWMSS0 , 50.0f ,SM_1_duty , SM_1_duty);
	    sleep(1);
	}

        BBBIO_ehrPWM_Disable(BBBIO_PWMSS0);

	iolib_free();
	return(0);
}


