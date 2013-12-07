/* LED GPIO Demo
 *
 * this demo express how to using BBBIO_GPIO_ prefix funcion to control BBB I/O .
 * if you switch on DIP swtich , the corresponding LED will be glittered .
 *
 *
 * pin_high and pin_low is very useful , but in some case , ex : 7-Segment display , it must pin high/low 8 times , it's very waste time .
 *
 * if you need to control many I/O in same time , please try to using BBBIO_GPIO_ prefix funcion ,
 * these function allow you control whole GPIO at once function call .
 *
 * NOTE : please check the pin set argument , it may cause some confuse in use  ,
	  ex : P8_46 is GPIO2[7] , so , you must using BBBIO_GPIO2 and BBBIO_GPIO_PIN_7 to control it ,
	       do no using BBBIO_GPIO_PIN_46 replace it , it's wrong .
*/
#include <stdio.h>
#include <stdlib.h>
#include "../../BBBio_lib/BBBiolib.h"
//--------------------------------------------------------------
int
main(void)
{
	iolib_init();
	BBBIO_sys_Enable_GPIO(BBBIO_GPIO2);

	BBBIO_GPIO_set_dir(BBBIO_GPIO2 ,
			   BBBIO_GPIO_PIN_10 |BBBIO_GPIO_PIN_11 | BBBIO_GPIO_PIN_12 |BBBIO_GPIO_PIN_13 ,	// Input
			   BBBIO_GPIO_PIN_6 | BBBIO_GPIO_PIN_7 | BBBIO_GPIO_PIN_8 | BBBIO_GPIO_PIN_9);		// Output

	int count =0;
	int DIPvalue=0 ;		// finger switch value
	int LEDvalue =0;
	while(count < 100)
	{
	    // Read DIP value
	    LEDvalue =0;
	    DIPvalue = BBBIO_GPIO_get(BBBIO_GPIO2 ,BBBIO_GPIO_PIN_10 |BBBIO_GPIO_PIN_11 | BBBIO_GPIO_PIN_12 |BBBIO_GPIO_PIN_13 );

	    // check value , and seting which LED must be glittered .
	    if(DIPvalue & BBBIO_GPIO_PIN_11)
		LEDvalue |=BBBIO_GPIO_PIN_7;

	    if(DIPvalue & BBBIO_GPIO_PIN_10)
		LEDvalue |=BBBIO_GPIO_PIN_6;

	    if(DIPvalue & BBBIO_GPIO_PIN_13)
		LEDvalue |=BBBIO_GPIO_PIN_9;

	    if(DIPvalue & BBBIO_GPIO_PIN_12)
		LEDvalue |=BBBIO_GPIO_PIN_8;

	    // glitter LED
	    BBBIO_GPIO_high(BBBIO_GPIO2 , LEDvalue);
	    iolib_delay_ms(100);

	    // close all
            BBBIO_GPIO_low(BBBIO_GPIO2 , BBBIO_GPIO_PIN_6 | BBBIO_GPIO_PIN_7 | BBBIO_GPIO_PIN_8 | BBBIO_GPIO_PIN_9);
	    iolib_delay_ms(100);
	    count ++;
	}
	iolib_free();
	return 0;
}


