
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#define __USE_GNU
#include <sched.h>

#include "../BBBio_lib/BBBiolib.h"
//-----------------------------------------------------------------
void Recover()
{
        for(j=0 ;j < 50 ; j++)
        {
            pin_high(8,12);
            pin_high(8,11);
            pin_high(8,15);
            BBBIO_sys_delay_us(1000 + i*100);

            pin_low(8,12);
            pin_low(8,11);
            pin_low(8,15);
            BBBIO_sys_delay_us(19000 - i*100);
        }

}
//-----------------------------------------------------------------
int
main(void)
{
	pid_t PID =0;
	struct sched_param param;
	int maxpri = sched_get_priority_max(SCHED_FIFO)+1;
	iolib_init();
	PID = getpid();
	sched_setscheduler(PID , SCHED_FIFO ,&param );

	int waiting =0 ;
	int count = 0;
	struct timespec a;
	struct timeval t_start,t_end;
	double uTime =0;

	iolib_setdir(8,11, BBBIO_DIR_OUT);
	iolib_setdir(8,12, BBBIO_DIR_OUT);
	iolib_setdir(8,15, BBBIO_DIR_OUT);
	//server motor

	int i =-3 , j ;

	Recover();
/*

	// button
	for(i=-3 ; i<= 6; i++)
	{
	    for(j=0 ;j < 20 ; j++)
	    {
		pin_high(8,12);
		BBBIO_sys_delay_us(1000 + i*100);

                pin_low(8,12);
                BBBIO_sys_delay_us(19000 - i*100);
 	    }
	    sleep(1);
	}

	// arm -1
        for(i=-3 ; i<= 6; i++)
        {
            for(j=0 ;j < 20 ; j++)
            {
                pin_high(8,11);
                BBBIO_sys_delay_us(1000 + i*100);
                pin_low(8,11);
                BBBIO_sys_delay_us(19000 - i*100);
            }
            sleep(1);
        }


        // arm -2
        for(i=-3 ; i<= 3; i++)
        {
            for(j=0 ;j < 20 ; j++)
            {
                pin_high(8,15);
                BBBIO_sys_delay_us(1000 + i*100);
                pin_low(8,15);
                BBBIO_sys_delay_us(19000 - i*100);
            }
            sleep(1);
        }


*/



	// open
/*
	printf("open\n");
	while(count <250)
	{
            pin_high(8,12);
	    BBBIO_sys_delay_us(2000);
	    pin_low(8,12);
	    BBBIO_sys_delay_us(8000);
	    count ++;
	}


	count =0;
	printf("close\n");
	// close
        while(count <250)
        {
            pin_high(8,12);
            BBBIO_sys_delay_us(1000);

            pin_low(8,12);
            BBBIO_sys_delay_us(9000);
            count ++;
        }


*/
	printf("pluse\n");

/*
	count =0;
	// pluse
        while(count <400)
        {
            pin_high(8,12);
            BBBIO_sys_delay_us(2000);

            pin_low(8,12);
            BBBIO_sys_delay_us(8000);
            count ++;
        }
	count =0;
        while(count <400)
        {
            pin_high(8,12);
            BBBIO_sys_delay_us(1500);

            pin_low(8,12);
            BBBIO_sys_delay_us(8500);
            count ++;
        }
*/

/*
	int i ;
	for(i= 9 ; i >0 ; i --)
	{
	    count=0 ;
	    printf("%d\n",i);
	    while(count <200)
	    {
		pin_high(8,12);
		BBBIO_sys_delay_us(2000 - i *100);

		pin_low(8,12);
		BBBIO_sys_delay_us(8000 + i* 100);
		count ++;
	    }

	}
*/

	printf("finish\n");

	iolib_free();
	return(0);
}


