
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "../BBBio_lib/BBBiolib.h"
//-----------------------------------------------------------------
int
main(void)
{
	iolib_init();

	int waiting =0 ;	// SIG status , 0 waiting SIG to High , 1 waiting SIG to LOW , 2 for receive finish
	int count = 0;
	struct timespec a;
	struct timeval t_start,t_end;
	double uTime =0;

	// test 10 times
	while(count <10)
	{
		iolib_setdir(8,12, BBBIO_DIR_OUT);
		// generate 2us pulse ,in default setting , pin_high / 0.2 us ,
		// nanosleep function may take same thing .
		pin_high(8,12);
		pin_high(8,12);
		pin_high(8,12);
		pin_high(8,12);
		pin_high(8,12);
                pin_high(8,12);
                pin_high(8,12);
                pin_high(8,12);
                pin_high(8,12);
                pin_high(8,12);

		pin_low(8,12);

		// waiting SIG pin signal return
		iolib_setdir(8,12, BBBIO_DIR_IN);
		waiting =0 ;

		while(waiting!=2)
		{
		    if (is_high(8,12) && waiting ==0)	//waiting SIG to HIGH
		    {
			gettimeofday(&t_start, NULL);
			waiting =1 ;
		    }

		    if (is_low(8,12) && waiting==1)	// waiting SIG to LOW
		    {
			gettimeofday(&t_end, NULL);
			waiting =2 ;
		    }
		}
		uTime = (t_end.tv_sec -t_start.tv_sec)*1000000.0 +(t_end.tv_usec -t_start.tv_usec);
		printf("%lf cm\n",(uTime /29.033)/2.0 );
		sleep(1);
		count ++;
	}
	iolib_free();
	return(0);
}


