/* Encoder Example (Quadrature Encoder Decoder)
 *
 * Example By:	Ali Yousuf
 * E-mail:	aly.yousuf7@gmail.com
 * Github:	https://github.com/alyyousuf7
 * Date:	2015-03-15
 *
 * This code shows how to get motor's shaft position with high CPR Encoders.
 * Connect encoder channel A and B to Beaglebone Black's GPIO P8_45 and P8_46.
*/
#include <stdio.h>
#include <stdlib.h>
#include "../../BBBio_lib/BBBiolib.h"
//--------------------------------------------------------------
int getState()
{
	int finalState = 0;
	int rawState = BBBIO_GPIO_get(BBBIO_GPIO2, BBBIO_GPIO_PIN_6 | BBBIO_GPIO_PIN_7);
	if(rawState & BBBIO_GPIO_PIN_6)
		finalState = 2;
	if(rawState & BBBIO_GPIO_PIN_7)
		finalState |= 1;
	return finalState;
}

int main(void)
{
	printf("Quadrature Encoder Pulse Decoder Example for BBBIOlib\n\n");
	
	int res = 0;
	res = iolib_init();
	if(res == -1) {
		printf("iolib init error");
		return -1;
	}
	res = BBBIO_sys_Enable_GPIO(BBBIO_GPIO2);
	if(res == -1) {
		printf("GPIO2 Enable Error");
		return -1;
	}

	res = BBBIO_GPIO_set_dir(BBBIO_GPIO2 ,
				   BBBIO_GPIO_PIN_6 | BBBIO_GPIO_PIN_7,	// Input
				   0);					// Output
	if(res == -1) {
		printf("Error in setting GPIO direction");
		return -1;
	}

	int lastposition = 0;
	int position = 0;
	int statePrev = getState();
	int stateCurr = statePrev;

	printf("Position: 0\n");
	while(1)
	{
		stateCurr = getState();
		if(statePrev == stateCurr) continue;

		switch(statePrev) {
			case 0:
				if(stateCurr == 1)
					position++;
				else if(stateCurr == 2)
					position--;
				break;
			case 1:
				if(stateCurr == 3)
					position++;
				else if(stateCurr == 0)
					position--;
				break;
			case 3:
				if(stateCurr == 2)
					position++;
				else if(stateCurr == 1)
					position--;
				break;
			case 2:
				if(stateCurr == 0)
					position++;
				else if(stateCurr == 3)
					position--;
				break;
		}
		statePrev = stateCurr;

		if(position != lastposition) {
			printf("Position: %i\n", position);
			lastposition = position;
		}
	}
	iolib_free();
	return 0;
}

