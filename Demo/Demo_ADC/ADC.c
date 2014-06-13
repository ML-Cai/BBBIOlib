
#include <stdio.h>
#include <stdlib.h>
#include "../../BBBio_lib/BBBiolib.h"
/* ----------------------------------------------------------- */
#define BUFFER_SIZE 100
#define SAMPLE_SIZE 10
/* ----------------------------------------------------------- */
int main(void)
{
	unsigned int sample;
	int i ,j;
	unsigned int buffer_AIN_0[BUFFER_SIZE] ={0};
	unsigned int buffer_AIN_1[BUFFER_SIZE] ={0};

	/* BBBIOlib init*/
	iolib_init();

	/* using ADC_CALC toolkit to decide the ADC module argument . Example Sample rate : 10000 sample/s
	 *
	 *	#./ADC_CALC -f 10000 -t 5
	 *
	 *	Suggest Solution :
	 *		Clock Divider : 160 ,   Open Dly : 0 ,  Sample Average : 1 ,    Sample Dly : 1
	 *
	 */
//	const int clk_div = 34 ;
	const int clk_div = 160;
	const int open_dly = 0;
	const int sample_dly = 1;

	/*ADC work mode : Timer interrupt mode
	 *	Note : This mode handle SIGALRM using signale() function in BBBIO_ADCTSC_work();
	 */
	BBBIO_ADCTSC_module_ctrl(BBBIO_ADC_WORK_MODE_TIMER_INT, clk_div);

	/*ADC work mode : Busy polling mode  */
	/* BBBIO_ADCTSC_module_ctrl(BBBIO_ADC_WORK_MODE_BUSY_POLLING, clk_div);*/


	BBBIO_ADCTSC_channel_ctrl(BBBIO_ADC_AIN0, BBBIO_ADC_STEP_MODE_SW_CONTINUOUS, open_dly, sample_dly, \
				BBBIO_ADC_STEP_AVG_1, buffer_AIN_0, BUFFER_SIZE);

	BBBIO_ADCTSC_channel_ctrl(BBBIO_ADC_AIN1, BBBIO_ADC_STEP_MODE_SW_CONTINUOUS, open_dly, sample_dly, \
				BBBIO_ADC_STEP_AVG_1, buffer_AIN_1, BUFFER_SIZE);
	

	//BBBIO_ADCTSC_module_ctrl(BBBIO_ADCTSC_module_ctrl(BBBIO_ADC_WORK_MODE_BUSY_POLLING, 1);

//	BBBIO_ADCTSC_channel_ctrl(BBBIO_ADC_AIN1, BBBIO_ADC_STEP_MODE_SW_CONTINUOUS, 0, 1, BBBIO_ADC_STEP_AVG_1, buffer_AIN_1, 100);

	for(i = 0 ; i < 3 ; i++) {
		printf("Start sample , fetch %d sample \n", BUFFER_SIZE);
		BBBIO_ADCTSC_channel_enable(BBBIO_ADC_AIN0);
		BBBIO_ADCTSC_channel_enable(BBBIO_ADC_AIN1);
		BBBIO_ADCTSC_work(SAMPLE_SIZE);

		printf("AIN 0 :\n");
		for(j = 0 ; j < SAMPLE_SIZE ; j++) {
			sample = buffer_AIN_0[j];
			printf("\t[sample : %d , %f v]\n", sample, ((float)sample / 4095.0f) * 1.8f);
		}
		printf("AIN 1 :\n");
		for(j = 0 ; j < SAMPLE_SIZE ; j++) {
			sample = buffer_AIN_1[j];
			printf("\t[sample : %d , %f v]\n", sample, ((float)sample / 4095.0f) * 1.8f);
                }
		printf("------------------------------\n");
		sleep(1);
	}

	iolib_free();
	return 0;
}


