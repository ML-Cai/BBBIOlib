#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#define __USE_GNU
#include <sched.h>
#include "../BBBio_lib/BBBiolib.h"
//-----------------------------------------------------------------
// S03T 2BBMG	0.33s /60 degree
float SM_speed = 0.33f / 60.0f ;	// rotate speed per degree (5.5ms)

const float SM_Cycle = 0.02 ;		//50HZ

float AngleArm_1 =-1 ;
float AngleArm_2 =-1 ;
float AngleButton =-1 ;
//-----------------------------------------------------------------
int GetPulseCycleCount(int angle)
{
	if (angle <0 || angle >180)	// S03T only have 180 angle
	    return -1;

	return (int)(((float)angle *SM_speed)/SM_Cycle +1) ;
}
//-----------------------------------------------------------------
int GetPulseCycleDelay(int angle)
{
	if (angle <0 || angle >180)     // S03T only have 180 angle
            return -1;

	// -3 ~ 12
	//angle /= 15 ;

	return (((int)((float)angle /12.0)-3) *100);
}
//-----------------------------------------------------------------
void Recover()
{
	int CycleCount ;
	int i ;
	int CycleDelay ;

	// ARM2
	CycleCount = GetPulseCycleCount(180) ;
	CycleDelay = GetPulseCycleDelay(0) ;
        for(i=0 ; i < CycleCount ; i++)
        {
            pin_high(8,15);
            BBBIO_sys_delay_us(1000 + CycleDelay);

            pin_low(8,15);
            BBBIO_sys_delay_us(19000 - CycleDelay);
        }

	// ARM1
	CycleCount = GetPulseCycleCount(180) ;
	CycleDelay = GetPulseCycleDelay(90) ;
        for(i=0 ; i < CycleCount ; i++)
        {
            pin_high(8,12);
            BBBIO_sys_delay_us(1000 + CycleDelay);

            pin_low(8,12);
            BBBIO_sys_delay_us(19000 - CycleDelay);
        }


	// Button
	CycleCount = GetPulseCycleCount(180) ;
	CycleDelay = GetPulseCycleDelay(0) ;
        for(i=0 ; i < CycleCount ; i++)
        {
            pin_high(8,11);
            BBBIO_sys_delay_us(1000 + CycleDelay);

            pin_low(8,11);
            BBBIO_sys_delay_us(19000 - CycleDelay);
        }

	AngleArm_2 =0;
	AngleArm_1 =90;
	AngleButton =0;
}
//-----------------------------------------------------------------
int
main(void)
{
	printf("%d %d %d %d\n" ,GetPulseCycleCount(180),GetPulseCycleDelay(180),GetPulseCycleCount(60),GetPulseCycleDelay(0) );
//	return 0;

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


