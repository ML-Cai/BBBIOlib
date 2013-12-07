
#include <stdio.h>
#include <stdlib.h>
#include "../../BBBio_lib/BBBiolib.h"
//-----------------------------------------------------------------
int Seven_Decode [] ={1,1,1,1,1,1,1,0 , // 0
		      0,1,1,0,0,0,0,0 , // 1
		      1,1,0,1,1,0,1,0 , // 2
                      1,1,1,1,0,0,1,0 , // 3
                      0,1,1,0,0,1,1,0 , // 4
                      1,0,1,1,0,1,1,0 , // 5
                      1,0,1,1,1,1,1,0 , // 6
                      1,1,1,0,0,0,0,0 , // 7
                      1,1,1,1,1,1,1,0 , // 8
                      1,1,1,1,0,1,1,0}; // 9

int Seven_Pin [] ={39,40,41,42,43,44,45,46};
//-----------------------------------------------------------------
void
Display_Number(int Number)
{
	Number %= 10 ;
	int * Dec_ptr = &Seven_Decode[Number*8];
	int i ;

	for( i=0; i< 8 ;i ++)
	{
	    if(*(Dec_ptr +i) ==0)
		pin_low(8,Seven_Pin[i]);
	   else
		pin_high(8,Seven_Pin[i]);
	}

}
//-----------------------------------------------------------------
int
main(void)
{
	int del =5;
	iolib_init();
	printf("init finish\n");
/*
	BBBIO_GPIO_set_dir(BBBIO_GPIO1 , 0 ,
					BBBIO_GPIO_PIN_12 |
					BBBIO_GPIO_PIN_13 |
                                        BBBIO_GPIO_PIN_14 |
                                        BBBIO_GPIO_PIN_15 );
*/
	BBBIO_sys_Enable_GPIO(BBBIO_GPIO2);

	iolib_setdir(8,11, BBBIO_DIR_OUT);	// Scan A
	iolib_setdir(8,12, BBBIO_DIR_OUT);	// Scan B
        iolib_setdir(8,15, BBBIO_DIR_OUT);	// Scan C
        iolib_setdir(8,16, BBBIO_DIR_OUT);	// Scan D

	iolib_setdir(8,39, BBBIO_DIR_OUT);      // Seg A
	iolib_setdir(8,40, BBBIO_DIR_OUT);      // Seg B
        iolib_setdir(8,41, BBBIO_DIR_OUT);      // Seg C
        iolib_setdir(8,42, BBBIO_DIR_OUT);      // Seg D
        iolib_setdir(8,43, BBBIO_DIR_OUT);      // Seg E
        iolib_setdir(8,44, BBBIO_DIR_OUT);      // Seg F
        iolib_setdir(8,45, BBBIO_DIR_OUT);      // Seg G
        iolib_setdir(8,46, BBBIO_DIR_OUT);      // Seg . (point)


	printf("OK");

	int count = 0;
	int DisplayNumber =1234 ;
	while(count < 200)
	{
		count ++ ;
		pin_low(8,11);
		Display_Number(DisplayNumber / 1000);
		iolib_delay_ms(del);
		pin_high(8,11);

                pin_low(8,12);
                Display_Number(DisplayNumber / 100);
                iolib_delay_ms(del);
                pin_high(8,12);

                pin_low(8,15);
                Display_Number(DisplayNumber / 10);
                iolib_delay_ms(del);
                pin_high(8,15);

                pin_low(8,16);
                Display_Number(DisplayNumber);
                iolib_delay_ms(del);
                pin_high(8,16);
	}
	BBBIO_sys_Disable_GPIO(BBBIO_GPIO2);
	iolib_free();
	return(0);
}


