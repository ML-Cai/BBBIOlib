#include <stdio.h>
#include <stdlib.h>
#include "../../BBBio_lib/BBBiolib.h"
/* -------------------------------------------------------------- */
int main(void)
{
	/* NOTICE :
	 *	please load BBBIO-EHRPWM overlay first
	 *	help : https://github.com/VegetableAvenger/BBBIOlib/tree/master/overlay
	 **/
	iolib_init();

	const float PWM_HZ = 100.0f ;	/* 100 Hz */
	const float duty_A = 20.0f ; 	/* 20% Duty cycle for PWM 0_A output */
	const float duty_B = 50.0f ;	/* 50% Duty cycle for PWM 0_B output*/

	printf("PWM Demo setting ....\n");
	BBBIO_PWMSS_Setting(BBBIO_PWMSS0, PWM_HZ ,duty_A , duty_B);

	printf("PWM %d enable for 10s ....\n", BBBIO_PWMSS0);
	BBBIO_ehrPWM_Enable(BBBIO_PWMSS0);
	sleep(10);

	BBBIO_ehrPWM_Disable(BBBIO_PWMSS0);
	printf("close\n");

	iolib_free();
	return 0;
}


