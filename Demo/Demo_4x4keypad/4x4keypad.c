
#include <stdio.h>
#include <stdlib.h>
#include "../../BBBio_lib/BBBiolib.h"
//-------------------------------------------------------------------------------------
// Real key deploy
const unsigned int key_map[4][4]={{'0','1','2','3'},
			 	  {'4','5','6','7'},
			 	  {'8','9','A','B'},
			 	  {'C','D','E','F'}};

// Key status ,in this demo , using pulling high configuration ,
//   non-push 	: 1
//   push 	: 0
unsigned int key_status[4][4]={0};
//-------------------------------------------------------------------------------------
void keypad_4x4_init()
{
        iolib_init();

        BBBIO_sys_Enable_GPIO(BBBIO_GPIO2);

	// P8_39 / P8_41 / P8_43 / P8_45 as input
	// P8_40 / P8_42 / P8_44 / P8_46 as input
	// GPIO mapping reference from BBB_SRM , page 80 , "Table 11. Expansion Header P8 Pinout"
        BBBIO_GPIO_set_dir(BBBIO_GPIO2 ,
                           BBBIO_GPIO_PIN_12 |BBBIO_GPIO_PIN_10 |BBBIO_GPIO_PIN_8 |BBBIO_GPIO_PIN_6 ,   // input
                           BBBIO_GPIO_PIN_13 |BBBIO_GPIO_PIN_11 |BBBIO_GPIO_PIN_9 |BBBIO_GPIO_PIN_7 );  // output

}
//-------------------------------------------------------------------------------------
void keypad_4x4_scan()
{
	int value=0;
	BBBIO_GPIO_low(BBBIO_GPIO2 ,BBBIO_GPIO_PIN_13);     //line 1
        value = BBBIO_GPIO_get(BBBIO_GPIO2 ,BBBIO_GPIO_PIN_12 |BBBIO_GPIO_PIN_10 |BBBIO_GPIO_PIN_8 |BBBIO_GPIO_PIN_6 );
        BBBIO_GPIO_high(BBBIO_GPIO2 ,BBBIO_GPIO_PIN_13);
        key_status[0][0] = value >> 6 & 0x01;
        key_status[0][1] = value >> 8 & 0x01;
        key_status[0][2] = value >> 10 & 0x01;
        key_status[0][3] = value >> 12 & 0x01;

        BBBIO_GPIO_low(BBBIO_GPIO2 ,BBBIO_GPIO_PIN_11);     //line 2
        value = BBBIO_GPIO_get(BBBIO_GPIO2 ,BBBIO_GPIO_PIN_12 |BBBIO_GPIO_PIN_10 |BBBIO_GPIO_PIN_8 |BBBIO_GPIO_PIN_6 );
        BBBIO_GPIO_high(BBBIO_GPIO2 ,BBBIO_GPIO_PIN_11);
        key_status[1][0] = value >> 6 & 0x01;
        key_status[1][1] = value >> 8 & 0x01;
        key_status[1][2] = value >> 10 & 0x01;
        key_status[1][3] = value >> 12 & 0x01;

        BBBIO_GPIO_low(BBBIO_GPIO2 ,BBBIO_GPIO_PIN_9);     //line 3
        value = BBBIO_GPIO_get(BBBIO_GPIO2 ,BBBIO_GPIO_PIN_12 |BBBIO_GPIO_PIN_10 |BBBIO_GPIO_PIN_8 |BBBIO_GPIO_PIN_6 );
        BBBIO_GPIO_high(BBBIO_GPIO2 ,BBBIO_GPIO_PIN_9);
        key_status[2][0] = value >> 6 & 0x01;
        key_status[2][1] = value >> 8 & 0x01;
        key_status[2][2] = value >> 10 & 0x01;
        key_status[2][3] = value >> 12 & 0x01;

        BBBIO_GPIO_low(BBBIO_GPIO2 ,BBBIO_GPIO_PIN_7);     //line 4
        value = BBBIO_GPIO_get(BBBIO_GPIO2 ,BBBIO_GPIO_PIN_12 |BBBIO_GPIO_PIN_10 |BBBIO_GPIO_PIN_8 |BBBIO_GPIO_PIN_6 );
        BBBIO_GPIO_high(BBBIO_GPIO2 ,BBBIO_GPIO_PIN_7);
        key_status[3][0] = value >> 6 & 0x01;
        key_status[3][1] = value >> 8 & 0x01;
        key_status[3][2] = value >> 10 & 0x01;
        key_status[3][3] = value >> 12 & 0x01;
}
//-------------------------------------------------------------------------------------
int
main(void)
{
	int count = 0;
	int i, j ;

	keypad_4x4_init();

	while(count <100)
	{
	    keypad_4x4_scan();

	    for(i=0 ; i< 4 ;i++)
	    {
            	for(j=0 ; j< 4 ;j++)
            	{
		    if(key_status[i][j]==0)
			printf("%c " ,key_map[i][j]);
		    else
		        printf("O ");
//		     printf("%c " ,key_status[i][j]+'0');
            	}
		printf("\n");
	    }
	    printf("========================\n");

            BBBIO_sys_delay_ms(100);
	    count ++;
	}

	iolib_free();
	return 0;
}


