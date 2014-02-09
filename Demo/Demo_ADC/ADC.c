
#include <stdio.h>
#include <stdlib.h>
#include "../../BBBio_lib/BBBiolib.h"
/* ----------------------------------------------------------- */
unsigned int buffer[100] ={0};
int main(void)
{
	unsigned int sample;
	int i ,j;

	/* BBBIOlib init*/
	iolib_init();

	BBBIO_ADCTSC_channel_ctrl(0, 1, 1, buffer, 100);
	BBBIO_ADCTSC_channel_enable(0);

	for(i = 0 ; i < 3 ; i++) {
		BBBIO_ADCTSC_channel_enable(0);
		BBBIO_ADCTSC_work(10);

		for(j = 0 ; j < 10 ; j++) {
			sample = buffer[j];
			printf("[sample : %d \t Data : %d \tChnnel : %d]\n", sample, sample & 0x0FFF, ( sample & 0xF0000)>>16 );
		}
		sleep(1);
		printf("------------------------------\n");
	}

	iolib_free();
	return 0;
}


