#include <stdio.h>
#include <stdlib.h>
#include "../BBBio_lib/BBBiolib.h"
//------------------------------------------------------------------------
#define ADT7301_DIN_PORT  8
#define ADT7301_DIN_PIN   11

#define ADT7301_CS_PORT   8
#define ADT7301_CS_PIN    16

#define ADT7301_DOUT_PORT 8
#define ADT7301_DOUT_PIN  15

#define ADT7301_SCLK_PORT 8
#define ADT7301_SCLK_PIN  12



int
ADT7301_read()
{
    int value = 0;
    int data_count =0;
    int i =0 ;

    pin_high(ADT7301_SCLK_PORT,ADT7301_SCLK_PIN);

    value =0;
    //set CS to 1 , why pin_low ? because CS in ADT7301 is inverse pin
    pin_low(ADT7301_CS_PORT,ADT7301_CS_PIN);

    //set DIN to 0
    pin_low(ADT7301_DIN_PORT,ADT7301_DIN_PIN);

    //read temperature Data
    for(i=0;i<16;i++)
    {
        //generate half cycle(1)
        pin_low(ADT7301_SCLK_PORT ,ADT7301_SCLK_PIN);
        iolib_delay_ms(1);

        //generate half cycle(0)
        pin_high(ADT7301_SCLK_PORT,ADT7301_SCLK_PIN);
        iolib_delay_ms(1);

        value <<= 1 ;
        int get_value = is_high(ADT7301_DOUT_PORT,ADT7301_DOUT_PIN) ;
        value |= get_value;
    }

    //set CS to 1
    pin_high(ADT7301_CS_PORT,ADT7301_CS_PIN);

    // return RAW value , this valuse is
    return value ;
}
//------------------------------------------------------------------------
int
main(void)
{
	int raw_temp  =0;
	int i= 0;
	iolib_init();
	iolib_setdir(8,11, BBBIO_DIR_OUT);
	iolib_setdir(8,12, BBBIO_DIR_OUT);
        iolib_setdir(8,15, BBBIO_DIR_IN);
        iolib_setdir(8,16, BBBIO_DIR_OUT);


	for(i=0 ;i< 10 ; i++)
	{
	    raw_temp = ADT7301_read();
	    printf("%f\n",(float)(raw_temp)/32.0f);
	    sleep(1);
	}
	iolib_free();
	return 0 ;
}
