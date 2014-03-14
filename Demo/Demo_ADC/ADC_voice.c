#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include "../../BBBio_lib/BBBiolib.h"

/* ------------------------------------------------------------ */
#define AUDIO_BUFFER_SIZE	44100
/* ----------------------------------------------------------- */
unsigned int buffer[AUDIO_BUFFER_SIZE] ={0};
/* ----------------------------------------------------------- */
int main(void)
{
	int i ,j;
	unsigned int *buf_ptr = &buffer[0];

	/* BBBIOlib init*/
	iolib_init();

	/* using ADC_CALC toolkit to decide the ADC module argument .
	 *
	 *	#./ADC_CALC -f 44100 -t 30
	 *
	 *	Suggest Solution :
	 *		Clock Divider : 34 ,    Open Dly : 1 ,  Sample Average : 1 ,    Sample Dly : 1
	 */
	const int clk_div = 34 ;
	const int open_dly = 1;
	const int sample_dly = 1;

	BBBIO_ADCTSC_module_ctrl(clk_div);
	BBBIO_ADCTSC_channel_ctrl(BBBIO_ADC_AIN0, BBBIO_ADC_STEP_MODE_SW_CONTINUOUS, open_dly, sample_dly, BBBIO_ADC_STEP_AVG_1, buffer, AUDIO_BUFFER_SIZE);

	struct timeval t_start,t_end;
 	float mTime =0;

	for(i = 0 ; i < 5 ; i++) {
		/* fetch data from ADC */
		BBBIO_ADCTSC_channel_enable(BBBIO_ADC_AIN0);

		gettimeofday(&t_start, NULL);
		BBBIO_ADCTSC_work(AUDIO_BUFFER_SIZE);
		gettimeofday(&t_end, NULL);

		/* **********************************************************
		*
		*	Add your Socket transmit function in this block
		*
		************************************************************* */

		mTime = (t_end.tv_sec -t_start.tv_sec)*1000000.0 +(t_end.tv_usec -t_start.tv_usec);
		mTime /=1000000.0f;
		printf("Sampling finish , fetch [%d] samples in %lfs\n", AUDIO_BUFFER_SIZE, mTime);
	}
	BBBIO_ehrPWM_Disable(BBBIO_PWMSS0);


	iolib_free();
	return 0;
}



