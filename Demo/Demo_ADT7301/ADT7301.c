#include <stdio.h>
#include <stdlib.h>
#include "../../BBBio_lib/BBBiolib.h"
//------------------------------------------------------------------------
int main(void)
{
        int raw_temp =0;
        int i= 0;
        iolib_init();

	if(BBBIO_McSPI_Enable(1)) {
	        BBBIO_McSPI_channel_ctrl(BBBIO_McSPI_SPI1 ,
        	                    BBBIO_McSPI_CH0 ,
                	            BBBIO_McSPI_Master ,
                        	    BBBIO_McSPI_TxRx ,
	                            0x8 ,                        // 48Mhz div 5 as SPI speed
                	            BBBIO_McSPI_CLK_MODE3 ,
        	                    BBBIO_McSPI_EN_ACT_LOW ,        // CS/En pin low when active
                        	    BBBIO_McSPI_D0O_D1I ,        // data0 output , data1 input
                        	    16);                        // 16 bit per work

	        for(i = 0 ; i < 20 ; i++) {
	                BBBIO_McSPI_work(1);
        	        raw_temp = BBBIO_McSPI_Rx(1,0) & 0x0000FFFF;
                	printf("MCSPI_RX0 , Raw : %X ,Temperature %f\n" ,raw_temp, raw_temp / 32.0f);
	                sleep(1);
        	}
	        BBBIO_McSPI_Disable(1);
	}
	else {
		printf("McSPI init failed\n");
	}
        iolib_free();
        return 0 ;

}
