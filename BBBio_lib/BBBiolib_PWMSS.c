#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include "BBBiolib.h"

extern int memh;
extern volatile unsigned int *ctrl_addr ;

const unsigned int PWMSS_AddressOffset[]={BBBIO_PWMSS0_ADDR,
					  BBBIO_PWMSS1_ADDR,
					  BBBIO_PWMSS2_ADDR};
//const unsigned int EPWM_AddressOffset[]={BBBIO_EPWM0_ADDR,
//					 BBBIO_EPWM1_ADDR,
//					 BBBIO_EPWM2_ADDR};
volatile unsigned int *pwmss_ptr[3]     ={NULL, NULL, NULL} ;
volatile unsigned int *epwm_ptr[3]      ={NULL, NULL, NULL} ;
volatile unsigned int *ecap_ptr[3]      ={NULL, NULL, NULL} ;
volatile unsigned int *eqep_ptr[3]      ={NULL, NULL, NULL} ;


#define TBCTL_CTRMODE_UP        0x0
#define TBCTL_CTRMODE_DOWN      0x1
#define TBCTL_CTRMODE_UPDOWN    0x2
#define TBCTL_CTRMODE_FREEZE    0x3

//-----------------------------------------------------------------------------------------------
/*********************************
 PWM init
 *******************************
 * iolib_init will run this function automatically 
 *
 *      @return         : 1 for success , 0 for failed
 *
 */

int BBBIO_PWM_Init()
{
	int i=0;
	if (memh ==0)
    	{
	    if (BBBIO_LIB_DBG) printf("BBBIO_PWM_Init: memory not mapped?\n");
		return 0;
    	}

	for (i=0; i<3; i++)
    	{
	    pwmss_ptr[i] = mmap(0 ,BBBIO_PWMSS_LEN ,PROT_READ | PROT_WRITE ,MAP_SHARED ,memh ,PWMSS_AddressOffset[i]);
       	    if(pwmss_ptr[i] == MAP_FAILED)
            {
		if (BBBIO_LIB_DBG) printf("BBBIO_PWM_Init: PWMSS %d mmap failure!\n",i);
                    return 0;
            }
	    ecap_ptr[i]  =(void *)pwmss_ptr[i] + 0x100 ;
	    eqep_ptr[i]  =(void *)pwmss_ptr[i] + 0x180 ;
	    epwm_ptr[i]  =(void *)pwmss_ptr[i] + 0x200 ;
    	}
	return 1;
}
//----------------------------------------------------------------------------------------------- 
/*********************************
 PWMSS status (no effect now)
 *******************************
 * set pluse rgument of epwm module
 *      @param PWMID    : EPWMSS number , 0~3
 *
 *      @return         : 1 for success , 0 for failed
 *
 */
int BBBIO_PWMSS_Status(unsigned int PWMID)
{
	int param_error=1;
	volatile unsigned int* reg;
	unsigned int reg_value ;

	if (memh==0)
            param_error=0;
    	if ((PWMID<0) || (PWMID>2))               // if input is not EPWMSS 0~ WPEMSS 2
            param_error=0;

    	if (param_error==0)
     	{
            if (BBBIO_LIB_DBG) printf("BBBIO_PWM_Status: parameter error!\n");
            	return 0;
    	}

	reg =(void*)ctrl_addr + BBBIO_PWMSS_CTRL;

	reg_value =*reg >> PWMID &0x01 ;
	if(reg_value ==0)
	{
	    printf("PWMSS %d Timebase clock Disable\n");
	}
	else
	{
	    reg=(void*)pwmss_ptr[PWMID] +BBBIO_PWMSS_CLKSTATUS;
//	    reg=(void*)pwmss_ptr[PWMID] +BBBIO_PWMSS_CLKCONFIG;
	    reg_value = *reg ;

	    printf("PWMSS [%d] :\tCLKSTOP_ACK %d , CLK_EN_ACK %d , CLKSTOP_ACK %d , CLK_EN_ACK %d , CLKSTOP_ACK %d , CLK_EN_ACK %d\n",
		    PWMID ,
		    reg_value >>9 & 0x1 ,
		    reg_value >>8 & 0x1 ,
		    reg_value >>5 & 0x1 ,
		    reg_value >>4 & 0x1 ,
		    reg_value >>1 & 0x1 ,
		    reg_value >>0 & 0x1 );
	}
	return 1 ;
}
//-----------------------------------------------------------------------------------------------
/*********************************
 PWMSS setting
 *******************************
 * set pluse rgument of epwm module
 *      @param PWMID    : EPWMSS number , 0~3
 *      @param HZ    	: pluse HZ
 *      @param dutyA    : Duty Cycle in ePWM A
 *      @param dutyB    : Duty Cycle in ePWM B
 *
 *      @return         : 1 for success , 0 for failed
 *
 *      @example        :  BBBIO_PWMSS_Setting(0 , 50.0f , 50.0f , 25.0f); 	// Generate 50HZ pwm in PWM0 ,
 *										// duty cycle is 50% for ePWM0A , 25% for ePWM0B
 */

int BBBIO_PWMSS_Setting(unsigned int PWMID , float HZ ,float dutyA ,float dutyB)
{
	int param_error=1;
	volatile unsigned short* reg16 ;
        if (memh==0)
            param_error=0;
        if ((PWMID<0) || (PWMID>2))              // if input is not EPWMSS 0~ WPEMSS 2
            param_error=0;
	if (HZ < 0 )
	    param_error=0;
	if(dutyA <0.0f || dutyA>100.0f || dutyB <0.0f || dutyB>100.0f)
	    param_error=0;

        if (param_error==0)
        {
            if (BBBIO_LIB_DBG) printf("BBBIO_PWMSS_Setting: parameter error!\n");
                return 0;
        }
	dutyA /=100.0f ;
	dutyB /=100.0f ;
	// compute neccessary TBPRD
	float Cyclens =0.0f ;
	float Divisor =0;
	int i , j ;
	const float CLKDIV_div[] = {1.0 ,2.0 ,4.0 ,8.0 ,16.0 ,32.0 , 64.0 , 128.0};
	const float HSPCLKDIV_div[] ={1.0 ,2.0 ,4.0 ,6.0 ,8.0 ,10.0 , 12.0 , 14.0};
	int NearCLKDIV =7;
	int NearHSPCLKDIV =7;
	int NearTBPRD =0;

	Cyclens = 1000000000.0f / HZ ; // 10^9 / HZ , comput time per cycle (ns)


	Divisor =  (Cyclens / 655350.0f) ;	// am335x provide /(128*14) divider , and per TBPRD means 10 ns when divider /1 ,
						// and max TBPRD is 65535 , so , the max cycle is 128*14* 65535 *10ns
	if(Divisor >(128*14))
	{
	    printf("BBBIO_PWMSS_Setting : Can't generate %f HZ \n",HZ);
	    return 0;
	}
	else
	{
            /* find an number nearst 65535 to improve duty precision,
             *
             * WHY?
	     * because using big TBPRD can increase the range of CMPA and CMPA ,
             * and it means we can get better precision on duty cycle.
             *
             *       EX : 20.25% duty cycle
             *			on TBPRD = 62500 , CMPA = 12656.25 ( .25 must rejection) , real duty : 20.2496% (12656 /62500)
             *			on TBPRD = 6250  , CMPA = 1265.625 ( .625 must rejection), real duty : 20.24%   (1265 6250)
             *			on TBPRD = 500   , CMPA = 101.25   ( .25 must rejection) , real duty : 20.2%    (101/500)
	     *
	     * Divisor = CLKDIV * HSPCLKDIV
	     *
             *     1 TBPRD : 10 ns
             * 65535 TBPRD : 655350 ns  ,
	     *
	     * 65535 TBPRD : 655350 * Divisor ns  = X TBPRD : Cyclens
	     *
	     * accrooding to that , we must find a Divisor value , let X nearest 65535 .
	     * so , Divisor must  Nearest Cyclens/655350
	     */

	    // using Exhaustive Attack metho
	    for(i=0 ; i< 8 ; i++)
	    {
		for(j=0 ; j< 8 ; j++)
            	{
		    if(((CLKDIV_div[i] * HSPCLKDIV_div[j]) > Divisor) &&
		       (CLKDIV_div[i] * HSPCLKDIV_div[j]) < (CLKDIV_div[NearCLKDIV] * HSPCLKDIV_div[NearHSPCLKDIV]))
		    {
			NearCLKDIV = i ;
			NearHSPCLKDIV = j ;
		    }
            	}
	    }
	    if (BBBIO_LIB_DBG)
		printf("nearest CLKDIV %f , HSPCLKDIV %f\n" ,CLKDIV_div[NearCLKDIV] ,HSPCLKDIV_div[NearHSPCLKDIV]);

	    NearTBPRD = (Cyclens / (10.0 *CLKDIV_div[NearCLKDIV] *HSPCLKDIV_div[NearHSPCLKDIV])) ;

	    if (BBBIO_LIB_DBG)
		printf("nearest TBPRD %d\n ",NearTBPRD);

/*
	    reg16=(void*)epwm_ptr[PWMID] +BBBIO_EPWM_DBRED ;
	    *reg16 = 0;

            reg16=(void*)epwm_ptr[PWMID] +BBBIO_EPWM_DBFED ;
            *reg16 = 0;

	    reg16=(void*)epwm_ptr[PWMID] +BBBIO_EPWM_DBCTL ;
            *reg16 = 0;


            reg16=(void*)epwm_ptr[PWMID] +BBBIO_EPWM_TBPHS ;
            *reg16=0;
            reg16=(void*)epwm_ptr[PWMID] +BBBIO_EPWM_TBCNT ;
            *reg16=0;
*/
	    reg16=(void*)epwm_ptr[PWMID] +BBBIO_EPWM_CMPB;              // duty cycle B
            *reg16 =(unsigned short)((float)NearTBPRD * dutyB);

            reg16=(void*)epwm_ptr[PWMID] +BBBIO_EPWM_CMPA;              // duty cycle A
            *reg16 =(unsigned short)((float)NearTBPRD * dutyA);

            reg16=(void*)epwm_ptr[PWMID] +BBBIO_EPWM_TBPRD;
            *reg16 =(unsigned short)NearTBPRD;

            reg16=(void*)epwm_ptr[PWMID] +BBBIO_EPWM_AQCTLA;
            *reg16 = (1 <<1) | ( 1 <<4) ;   //ZRO : AQ_SET
                                            //CAU : AQ_CLEAR

	    reg16=(void*)epwm_ptr[PWMID] +BBBIO_EPWM_AQCTLB;
            *reg16 = (1 <<1) | ( 1 <<8) ;   //ZRO : AQ_SET

            reg16=(void*)epwm_ptr[PWMID] +BBBIO_EPWM_TBCTL;
//          *reg16 = TBCTL_CTRMODE_FREEZE | (NearCLKDIV <<10) | (NearHSPCLKDIV <<7);    //setting divisor and freeze
            *reg16 = TBCTL_CTRMODE_UP | (NearCLKDIV <<10) | (NearHSPCLKDIV <<7);
                                            //CBU : AQ_CLEAR

	}

	return 1;
}
//-----------------------------------------------------------------------------------------------
/*********************************
 Enable PWMSS module
 *******************************
 * Enable ePWM module
 *      @param PWMID    : PWMSS number , 0~2
 *
 *      @return         : void
 *
 *      @example        : BBBIO_PWMSS_Enable(0) ;// Enable PWMSS 0
 *
 */

void BBBIO_PWMSS_Enable(unsigned int PWMID)
{
	volatile unsigned short* reg16 ;
        reg16=(void*)epwm_ptr[PWMID] +BBBIO_EPWM_TBCTL;
	*reg16 &= ~0x3;

        reg16=(void*)epwm_ptr[PWMID] +BBBIO_EPWM_DBCTL ;
        printf("DBCTL %X ,",*reg16);

	reg16=(void*)epwm_ptr[PWMID] +BBBIO_EPWM_DBRED ;
	printf("DBRED %X ,",*reg16);

        reg16=(void*)epwm_ptr[PWMID] +BBBIO_EPWM_DBFED ;
        printf("DBFED %X\n",*reg16);

}
//--------------------------------------------------------
/*********************************
 Disable PWMSS module
 *******************************
 * Disble ePWM module
 *      @param PWMID    : PWMSS number , 0~2
 *
 *      @return         : void
 *
 *      @example        : BBBIO_PWMSS_Disable(0) ;// Disable PWMSS 0
 *
 */

void BBBIO_PWMSS_Disable(unsigned int PWMID)
{
 	volatile unsigned short* reg16 ;
        reg16=(void*)epwm_ptr[PWMID] +BBBIO_EPWM_TBCTL;
        *reg16 |= 0x3;
}
//--------------------------------------------------------












