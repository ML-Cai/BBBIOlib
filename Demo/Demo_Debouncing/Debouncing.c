/* Debouncing Demo for Beaglebone black
 *
 * this demo show an simple demo for Buttom Debouncing . it display push count on 2~3's .
 * you can compare the version of Debouncing and non-debouncing , accroding to comment DEBOUNCING define
 */
#include <stdio.h>
#include <stdlib.h>
#include "../../BBBio_lib/BBBiolib.h"
//------------------------------------------------
#define DEBOUNCING_BUTTON_DOWN	0x01
#define DEBOUNCING_BUTTON_UP	0x02

// comment this define will cause non-debouncing mode
#define DEBOUNCING
//-----------------------------------------------
int main(void)
{
	int DebFlag  =DEBOUNCING_BUTTON_UP;
	int DebCount = 0;
	iolib_init();

	iolib_setdir(8,12, BBBIO_DIR_IN);
	iolib_setdir(8,11, BBBIO_DIR_IN);

#ifdef DEBOUNCING
        BBBIO_sys_Enable_Debouncing(8 ,11 ,10);
#else
	BBBIO_sys_Disable_Debouncing(8 ,11);
#endif

	printf("OK\n");

        int count = 0;
	int Test =0;
        while(Test <10)
        {
                count ++ ;
                if (is_high(8,11))
                {
		    DebFlag = DEBOUNCING_BUTTON_DOWN;
                }
		else
                {
		    if(DebFlag == DEBOUNCING_BUTTON_DOWN)
		    {
			DebCount ++ ;
			DebFlag = DEBOUNCING_BUTTON_UP ;
		    }
                }
		if(count == 10000000)
		{
		    printf("%d\n",DebCount);
		    count =0;
		    Test ++;
		}
        }
	BBBIO_sys_Disable_Debouncing(8 ,11);
        iolib_free();
	printf("Release\n");
        return(0);
}


