
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "../BBBio_lib/BBBiolib.h"
//-----------------------------------------------------------------
int
main(void)
{
	int del =5;
	iolib_init();

	int waiting =0 ;
	int count = 0;
	struct timespec a;
	struct timeval t_start,t_end;
	double uTime =0;

	while(count <10)
	{
		iolib_setdir(8,12, BBBIO_DIR_OUT);
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
		iolib_setdir(8,12, BBBIO_DIR_IN);
		waiting =0 ;

		while(waiting!=2)
		{
		    if (is_high(8,12) && waiting ==0)
		    {
			gettimeofday(&t_start, NULL);
			waiting =1 ;
		    }

		    if (is_low(8,12) && waiting==1)
		    {
			gettimeofday(&t_end, NULL);
			waiting =2 ;
		    }
		}
		uTime = (t_end.tv_sec -t_start.tv_sec)*1000000.0 +(t_end.tv_usec -t_start.tv_usec);
		printf("%lf us\n",(uTime /29.033)/2.0 );
		sleep(1);
		count ++;
	}
	iolib_free();
	return(0);
}


