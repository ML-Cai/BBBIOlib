
#include <stdio.h>
#include <stdlib.h>
#include "../../BBBio_lib/BBBiolib.h"
/* ----------------------------------------------------------- */
int main(void)
{
	unsigned int sample;
	int i ,j;
	unsigned int buffer_AIN_0[100] ={0};
	unsigned int buffer_AIN_1[100] ={0};

	/* BBBIOlib init*/
	iolib_init();

	/*ADC work mode : Busy polling mode  */
	//BBBIO_ADCTSC_module_ctrl(BBBIO_ADC_WORK_MODE_BUSY_POLLING, 1);
	BBBIO_ADCTSC_module_ctrl(BBBIO_ADC_WORK_MODE_TIMER_INT, 1);

	/*ADC work mode : Timer interrupt mode
	 *	Note : This mode handle SIGALRM using signale() function in BBBIO_ADCTSC_work();
	 */
	//BBBIO_ADCTSC_module_ctrl(BBBIO_ADCTSC_module_ctrl(BBBIO_ADC_WORK_MODE_BUSY_POLLING, 1);

	BBBIO_ADCTSC_channel_ctrl(BBBIO_ADC_AIN0, BBBIO_ADC_STEP_MODE_SW_CONTINUOUS, 0, 1, BBBIO_ADC_STEP_AVG_1, buffer_AIN_0, 100);
	BBBIO_ADCTSC_channel_enable(BBBIO_ADC_AIN0);

	BBBIO_ADCTSC_channel_ctrl(BBBIO_ADC_AIN1, BBBIO_ADC_STEP_MODE_SW_CONTINUOUS, 0, 1, BBBIO_ADC_STEP_AVG_1, buffer_AIN_1, 100);
	BBBIO_ADCTSC_channel_enable(BBBIO_ADC_AIN1);

	for(i = 0 ; i < 3 ; i++) {
		printf("Start sample , fetch 10 sample \n");
		BBBIO_ADCTSC_work(10);

		printf("Channel 0 :\n");
		for(j = 0 ; j < 10 ; j++) {
			sample = buffer_AIN_0[j];
			printf("\t[sample : %d , %f v]\n", sample, ((float)sample / 4095.0f) * 1.8f);
		}
		printf("Channel 1 :\n");
		for(j = 0 ; j < 10 ; j++) {
			sample = buffer_AIN_1[j];
			printf("\t[sample : %d , %f v]\n", sample, ((float)sample / 4095.0f) * 1.8f);
                }
		printf("------------------------------\n");
		sleep(1);
	}

	iolib_free();
	return 0;
}


