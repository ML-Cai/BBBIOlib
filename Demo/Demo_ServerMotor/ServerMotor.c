
#include <stdio.h>
#include <stdlib.h>
#include "../../BBBio_lib/BBBiolib.h"

int
main(void)
{
	iolib_init();

/*
        iolib_setdir(8,12, BBBIO_DIR_OUT);
        int i=1 ,count;
        for(i= 9 ; i >0 ; i --)
        {
            count=0 ;
            printf("%d\n",i);
            while(count <1000)
            {
                pin_high(8,12);
                BBBIO_sys_delay_us(2000 - i *100);

                pin_low(8,12);
                BBBIO_sys_delay_us(8000 + i* 100);
                count ++;
            }

        }
*/


	int i=9;
	float SM_1_duty =0;

	BBBIO_PWMSS_Disable(0);
        for(i=0 ;i<= 90 ;i++)
        {
	    SM_1_duty = 11.0f + i*0.1 ;
            printf("%d , %f\n", i ,SM_1_duty);

	    BBBIO_PWMSS_Setting(0 , 100.0f ,SM_1_duty ,SM_1_duty );
	    sleep(1);
	}

	BBBIO_PWMSS_Disable(0);


/*
	for(i=60 ;i<= 140 ;i+=10)
	{
	    SM_1_duty  = 4.0f + i* 0.04444f ;
	    printf("Angle : %d , duty : %f\n" ,i ,SM_1_duty);
	    BBBIO_PWMSS_Setting(0 , 50.0f ,4.2f , 4.2);
            BBBIO_PWMSS_Setting(1 , 50.0f ,SM_1_duty , SM_1_duty);
	    BBBIO_PWMSS_Setting(2 , 50.0f ,4.2f , 4.2);


//            BBBIO_PWMSS_Enable(0);
//            BBBIO_PWMSS_Enable(1);
//            BBBIO_PWMSS_Enable(2);

            sleep(3);
//            BBBIO_PWMSS_Disable(0);
//            BBBIO_PWMSS_Disable(1);
//            BBBIO_PWMSS_Disable(2);

	}
 */
            BBBIO_PWMSS_Disable(1);
            BBBIO_PWMSS_Disable(2);

	iolib_free();
	return(0);
}


