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
#define AUDIO_SAMPLE_RATE	44100
/* ----------------------------------------------------------- */
unsigned int buffer[AUDIO_BUFFER_SIZE*2] ={0};
int buf_size = 0;
/* ----------------------------------------------------------- */
#define NET_PORT	5005
#define NET_HOST	"YOUR IP"	/* your IP */
#define NET_BUFFER_SIZE 1028
/* ----------------------------------------------------------- */
void Tx_loop(void *argv)
{
	int Tx_socket =-1;
	struct sockaddr_in dest;
	char net_buffer[NET_BUFFER_SIZE] ={0};

	dest.sin_family = AF_INET;
	dest.sin_port = htons(NET_PORT);
	inet_aton(NET_HOST, &dest.sin_addr);dest.sin_family = AF_INET;

	Tx_socket = socket(AF_INET, SOCK_DGRAM, 0);
	if(Tx_socket < 0) {
		fprintf(stderr, "socket error\n");
	}

	while(1) {
		if(buf_size >0) {
			unsigned char * buf_ptr = (unsigned char *)buffer;
			int offset =1024 ;
			int ID =0;
			int frame_size = buf_size;
			while(frame_size >0) {
				memcpy(&net_buffer[0], &ID, sizeof(int));
				memcpy(&net_buffer[4], buf_ptr + ID * 1024, sizeof(char) * offset);
				sendto(Tx_socket, &net_buffer[0], (4 + offset) , 0, (struct sockaddr *)&dest, sizeof(dest));
				frame_size -= 1024 ;
				ID ++ ;
				if(frame_size <1024) offset = frame_size ;
			}
			buf_size = 0;
		}
		usleep(10);
	}
	close(Tx_socket);
}
/* ----------------------------------------------------------- */
int main(void)
{
	int i ,j;
	unsigned int *buf_ptr = &buffer[0];
	struct timeval t_start, t_end;
 	float mTime =0;
	int Tx_thread ;

	/* BBBIOlib init*/
	iolib_init();
	iolib_setdir(8,12, BBBIO_DIR_IN);

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

	BBBIO_ADCTSC_module_ctrl(BBBIO_ADC_WORK_MODE_TIMER_INT, clk_div);
	BBBIO_ADCTSC_channel_ctrl(BBBIO_ADC_AIN0, BBBIO_ADC_STEP_MODE_SW_CONTINUOUS, open_dly, \
				sample_dly, BBBIO_ADC_STEP_AVG_1, buffer, AUDIO_BUFFER_SIZE);

	pthread_create(&Tx_thread, NULL, &Tx_loop, NULL);

	for(i = 0 ; i < 100 ; i++) {
		/* fetch data from ADC */
		printf("sample\n");

		BBBIO_ADCTSC_channel_enable(BBBIO_ADC_AIN0);
		gettimeofday(&t_start, NULL);
		BBBIO_ADCTSC_work(AUDIO_BUFFER_SIZE);
		gettimeofday(&t_end, NULL);

		/* set transmit data size */
		buf_size = sizeof(int) * AUDIO_BUFFER_SIZE;

		mTime = (t_end.tv_sec -t_start.tv_sec)*1000000.0 +(t_end.tv_usec -t_start.tv_usec);
		mTime /=1000000.0f;
		printf("Sampling finish , fetch [%d] samples in %lfs\n", AUDIO_BUFFER_SIZE, mTime);
	}
	printf("finish\n");
	iolib_free();
	return 0;
}



