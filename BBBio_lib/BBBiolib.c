// BBBiolib.c
// Simple I/O library of Beaglebone balck
// 
// this fnnction is under construction , so mix some libio and BBBIO express ,
// libio support some basic function for Beaglebone black I/O .
// BBBIO support I/O function using GPIO as unit .
// all function will integrated as soon as fast .
//
// v1 	October 2013 - shabaz (iolib)
// v2 	October 2013 - shabaz (iolib)
// V2.1 November 2013 - VagetableAvenger (BBBlib)
// V2.2 November 2013 - VagetableAvenger (BBBlib) : add GPIO Enable/Disable function

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include "BBBiolib.h"

// Memory mapping offset if GPIO , means the memory address of GPIOs
const unsigned int GPIO_AddressOffset[]={BBBIO_GPIO0_ADDR, BBBIO_GPIO1_ADDR, BBBIO_GPIO2_ADDR, BBBIO_GPIO3_ADDR};

// GPIO Port number set of Beaglebone Black P8  ,
// -1 as GND or VCC , 0 / 1 / 2 for GPIO 0/1/2
const char p8_PortSet[]={-1,-1, 1, 1, 1, 1, 2, 2,
                         2 , 2, 1, 1, 0, 0, 1, 1,
                         0 , 2, 0, 1, 1, 1, 1, 1,
                         1 , 1, 2, 2, 2, 2, 0, 0, 
                         0 , 2, 0, 2, 2, 2, 2, 2,
                         2 , 2, 2, 2, 2, 2};

//  GPIO Port ID set of Beaglebone Black P8  ,
// 0 as GND , offset X as GPIO ID m this value must combine with GPIO number
const unsigned int p8_PortIDSet[]={
					0		,0		,1<<6	,1<<7	,1<<2	,1<<3	,1<<2	,1<<3, 
					1<<5	,1<<4	,1<<13	,1<<12	,1<<23	,1<<26	,1<<15	,1<<14,
					1<<27	,1<<1	,1<<22	,1<<31	,1<<30	,1<<5	,1<<4	,1<<1,
					1<<0	,1<<29	,1<<22	,1<<24	,1<<23	,1<<25	,1<<10	,1<<11,
					1<<9	,1<<17	,1<<8	,1<<16	,1<<14	,1<<15	,1<<12	,1<<13,
					1<<10	,1<<11	,1<<8	,1<<9	,1<<6	,1<<7};

// GPIO Port number set of Beaglebone Black P9  ,
// -1 as GND or VCC , 0 / 1 / 2 for GPIO 0/1/2
const char p9_PortSet[]={-1,-1,-1,-1,-1,-1,-1,-1,
	                     -1,-1, 0, 1, 0, 1, 1, 1, 
	                      0, 0, 0, 0, 0, 0, 1,-1, 
	                      3,-1, 3, 3, 3,-1, 3,-1,
	                     -1,-1,-1,-1,-1,-1,-1,-1,
	                      0, 0,-1,-1,-1,-1};

//  GPIO Port ID set of Beaglebone Black P9  ,
// 0 as GND , offset X as GPIO ID m this value must combine with GPIO number							  
const unsigned int p9_PortIDSet[]={
					0		,0		,0		,0		,0		,0		,0		,0,
					0		,0		,1<<30	,1<<28	,1<<31	,1<<18	,1<<16	,1<<19, 
					1<<5	,1<<4	,1<<13	,1<<12	,1<<3	,1<<2	,1<<17	,0,
					1<<21	,0		,1<<19	,1<<17	,1<<15	,0		,1<<14	,0,
					0		,0		,0		,0		,0		,0		,0		,0,
					1<<20	,1<<7	,0		,0		,0		,0};
	    
// Memory Handle and Control Handle		
int memh=0;
int ctrlh=0;
volatile unsigned int *gpio_addr[4]={NULL, NULL, NULL, NULL};
volatile unsigned int *ctrl_addr=NULL;
volatile unsigned int *cm_per_addr=NULL;
volatile unsigned int *cm_wkup_addr=NULL ;

// pointer to const Port set and Port ID set array
char* PortSet_ptr[2];
unsigned int* PortIDSet_ptr[2];
//-----------------------------------------------------------------------------------------------
// ********************************
// Library Init
// ********************************
int
iolib_init(void)
{
	int i;

	if (memh)
	{
		if (BBBIO_LIB_DBG) printf("iolib_init: memory already mapped?\n");
		return(-1);
	}

	PortSet_ptr[0]=(char*)p8_PortSet;
	PortSet_ptr[1]=(char*)p9_PortSet;
	PortIDSet_ptr[0]=(unsigned int*)p8_PortIDSet;
	PortIDSet_ptr[1]=(unsigned int*)p9_PortIDSet;

	// using memory mapped I/O
	memh=open("/dev/mem", O_RDWR);

	// mapping Clock Module Peripheral Registers

	cm_per_addr = mmap(0, BBBIO_CM_PER_LEN, PROT_READ | PROT_WRITE, MAP_SHARED, memh, BBBIO_CM_PER_ADDR);
	if(cm_per_addr == MAP_FAILED)
    	{
            if (BBBIO_LIB_DBG) printf("iolib_init: CM_PER mmap failure! ,error :%s\n" ,strerror(errno));
            return(-1);
	}


	// mapping Clock Module Wakeup Registers
	//
	/*  Note : the reason why not using mmap direct to mapping cm_wkup is page boundar .
	 * 	   if using mmap directlly (like CM_PER ), it will cause an error about EINVAL (invalid argument) .
	 *	   because the address of CM_WKUP is 0x44E00400 , it doesn't align the page (4K).
	*/
	cm_wkup_addr =(void *)cm_per_addr + BBBIO_CM_WKUP_OFFSET_FROM_CM_PER ;



	// mapping Address of GPIO 0~4
	for (i=0; i<4; i++)
	{
		gpio_addr[i] = mmap(0 ,BBBIO_GPIOX_LEN ,PROT_READ | PROT_WRITE ,MAP_SHARED ,memh ,GPIO_AddressOffset[i]); 
		if(gpio_addr[i] == MAP_FAILED)
		{
			if (BBBIO_LIB_DBG) printf("iolib_init: gpio mmap failure!\n");
				return(-1);
		}
	}

	// mapping Control Module Registers
	// for pin mux control , not implemented today
	if (BBBIO_PINMUX_EN)
	{
		ctrl_addr = mmap(0, CONTROL_LEN, PROT_READ | PROT_WRITE, MAP_SHARED, ctrlh, CONTROL_MODULE);
		if(ctrl_addr == MAP_FAILED)
		{
				if (BBBIO_LIB_DBG) printf("iolib_init: control module mmap failure!\n");
			return(-1);
		}
	}
	// set the bit in the OE register in the appropriate region
	if (BBBIO_LIB_DBG)
	{
		for (i=0; i<4; i++)
		{
			printf("mmap region %d address is 0x%08x\n", i, gpio_addr[i]);
		}
	}
	return(0);
}
//-----------------------------------------------------------------------------------------------
// ********************************
// Library free
// ********************************
int
iolib_free(void)
{
	if (memh!=0)
	{
		close(memh);
	}
	return(0);
}
//-----------------------------------------------------------------------------------------------
// ********************************
// Set I/O direction (Input/Output)
// ********************************
int
iolib_setdir(char port, char pin, char dir)
{
	int i;
	int param_error=0;				// parameter error 
	volatile unsigned int* reg;		// GPIO register
	int reg_port = port -8;			// port number of register process 
	int reg_pin = pin -1;			// pin id of register process 

	// sanity checks
	if (memh==0)
		param_error=1;
	if ((port<8) || (port>9))		// if input is not port8 and port 9 , because BBB support P8/P9 Connector
		param_error=1;
	if ((pin<1) || (pin>46))		// if pin over/underflow , range : 1~46
		param_error=1;
	if (PortSet_ptr[port][pin]<0)	// pass GND OR VCC (PortSet as -1)
		param_error=1;

	if (param_error)
	{
		if (BBBIO_LIB_DBG) printf("iolib_setdir: parameter error!\n");
		return(-1);
	}

	if (BBBIO_LIB_DBG)
		printf("iolib_setdir: PortSet_ptr P%d.%d , %X\n",port ,pin , PortSet_ptr[port-8][pin-1]);

	reg=(void*)gpio_addr[PortSet_ptr[port-8][pin-1]] +BBBIO_GPIO_OE;

	if (dir ==BBBIO_DIR_OUT)
	{
		*reg &= ~(PortIDSet_ptr[port-8][pin-1]);
	}
	else if (dir ==BBBIO_DIR_IN)
	{
		*reg |= PortIDSet_ptr[port-8][pin-1];
	}

	return(0);
}
//-----------------------------------------------------------------------------------------------
inline void
pin_high(char port, char pin)
{
	*((unsigned int *)((void *)gpio_addr[PortSet_ptr[port-8][pin-1]]+BBBIO_GPIO_SETDATAOUT)) = PortIDSet_ptr[port-8][pin-1];
}
//-----------------------------------------------------------------------------------------------
inline void
pin_low(char port, char pin)
{
	*((unsigned int *)((void *)gpio_addr[PortSet_ptr[port-8][pin-1]]+BBBIO_GPIO_CLEARDATAOUT)) = PortIDSet_ptr[port-8][pin-1];
}
//-----------------------------------------------------------------------------------------------
inline char
is_high(char port, char pin)
{
//	printf("IN :%X\n",*((unsigned int *)((void *)gpio_addr[PortSet_ptr[port-8][pin-1]]+BBBIO_GPIO_DATAIN)));
//	printf("OUT :%X\n",*((unsigned int *)((void *)gpio_addr[PortSet_ptr[port-8][pin-1]]+BBBIO_GPIO_DATAOUT)));
	return ((*((unsigned int *)((void *)gpio_addr[PortSet_ptr[port-8][pin-1]]+BBBIO_GPIO_DATAIN)) & PortIDSet_ptr[port-8][pin-1])!=0);
}
//-----------------------------------------------------------------------------------------------
inline char
is_low(char port, char pin)
{
//	printf("IN :%X\n",*((unsigned int *)((void *)gpio_addr[PortSet_ptr[port-8][pin-1]]+BBBIO_GPIO_DATAIN)));
//        printf("OUT :%X\n",*((unsigned int *)((void *)gpio_addr[PortSet_ptr[port-8][pin-1]]+BBBIO_GPIO_DATAOUT)));
	return ((*((unsigned int *)((void *)gpio_addr[PortSet_ptr[port-8][pin-1]]+BBBIO_GPIO_DATAIN)) & PortIDSet_ptr[port-8][pin-1])==0);
}
//-----------------------------------------------------------------------------------------------
int
iolib_delay_ms(unsigned int msec)
{
	int ret;
	struct timespec a;
	if (msec>999)
	{
		fprintf(stderr, "delay_ms error: delay value needs to be less than 999\n");
		msec=999;
	}
	a.tv_nsec=((long)(msec))*1E6d;
	a.tv_sec=0;
	if ((ret = nanosleep(&a, NULL)) != 0)
	{
		fprintf(stderr, "delay_ms error: %s\n", strerror(errno));
	}
	return(0);
}

//=============================================================================================
// BBB I/O new function 
// All function below is unstable

//-----------------------------------------------------------------------------------------------
/*********************************
 GPIO Set direction
 *******************************
 * Set whole GPIO transmission direction
 *	@param gpio		: GPIO number , BBBIO_GPIO0 / BBBIO_GPIO1 / BBBIO_GPIO2 / BBBIO_GPIO3 .
 *	@param inset	: Inpute pin set , using BBBIO_GPIO_PIN_? and | operator to express it.
 *					  The pin which not selected will keeping origin status .
 *	@param outset	: Output pin set , format as same as inset .
 *
 *	@return : 1 for success , 0 for failed
 *
 *	@example : BBBIO_GPIO_set_dir(BBBIO_GPIO1 ,
 *								 BBBIO_GPIO_PIN_11 | BBBIO_GPIO_PIN_12 ,
 *								 BBBIO_GPIO_PIN_15 | BBBIO_GPIO_PIN_16);
 *
 *	Warring : if you nedd this function , please check value you input , or it may effect other chip or device
 */

int
BBBIO_GPIO_set_dir(unsigned int  gpio, unsigned int inset , unsigned int outset)
{
	int i;
	int param_error=0;				// parameter error 
	volatile unsigned int* reg;		// GPIO register
	
	// sanity checks
	if (memh==0)
		param_error=1;
		
	if (gpio <0 || gpio >3)	//GPIO range
		param_error=1;
		
	if (inset==0 && outset==0)				// pin identify error
		param_error=1;
		
	if (param_error)
	{
		if (BBBIO_LIB_DBG) printf("BBBIO_GPIO_set_dir: parameter error!\n");
		return(-1);
	}
	
	reg=(void*)gpio +BBBIO_GPIO_OE;
	*reg &= ~outset ;
	*reg |= inset ;
	
	return 0;
}
//-----------------------------------------------------------------------------------------------
/*********************************
 Check GPIO clock status
 *******************************
 * Display GPIO status , this function just display the register value bellow:
 *	CM_PER_GPIO1_CLKCTRL
 *	CM_PER_GPIO2_CLKCTRL
 *	CM_PER_GPIO3_CLKCTRL
 *
 * Register format in AM335x Technical Reference Manual ,
 *		CM_PER_GPIO1_CLKCTRL , session 8.1.12.1.29 , page:947
 *		CM_PER_GPIO2_CLKCTRL , session 8.1.12.1.30 , page:948
 *		CM_PER_GPIO3_CLKCTRL , session 8.1.12.1.31 , page:949
*/
void
BBBIO_sys_GPIO_CLK_status()
{
	volatile unsigned int* reg ;
	unsigned int reg_value =0;
	
	reg =(void*)cm_per_addr + BBBIO_CM_PER_L4LS_CLKSTCTRL;
	reg_value = *reg ;
	printf("CM_PER CM_PER_L4LS_CLKSTCTRL : %X\n\n", reg_value );
	
	reg =(void*)cm_per_addr + BBBIO_CM_PER_GPIO1_CLKCTRL;
	reg_value = *reg ;
	printf("CM_PER CM_PER_GPIO1_CLKCTRL : %X\n", *reg );
	printf("\t[18]    OPTFCLKEN_GPIO_1_GDBCLK : %X\n", (reg_value >>18) & 0x01 );
	printf("\t[17-18] IDLEST                  : %X\n", (reg_value >>16) & 0x02 );
	printf("\t[0-1]   MODULEMODE              : %X\n\n", reg_value & 0x02 );

	reg =(void*)cm_per_addr + BBBIO_CM_PER_GPIO2_CLKCTRL;
	reg_value = *reg ;
	printf("CM_PER CM_PER_GPIO2_CLKCTRL : %X\n", *reg );
	printf("\t[18]    OPTFCLKEN_GPIO_2_GDBCLK : %X\n", (reg_value >>18) & 0x01 );
	printf("\t[17-18] IDLEST                  : %X\n", (reg_value >>16) & 0x02 );
	printf("\t[0-1]   MODULEMODE              : %X\n\n", reg_value & 0x02 );

	reg =(void*)cm_per_addr + BBBIO_CM_PER_GPIO3_CLKCTRL;
	reg_value = *reg ;
	printf("CM_PER CM_PER_GPIO3_CLKCTRL : %X\n", *reg );
	printf("\t[18]    OPTFCLKEN_GPIO_3_GDBCLK : %X\n", (reg_value >>18) & 0x01 );
	printf("\t[17-18] IDLEST                  : %X\n", (reg_value >>16) & 0x02 );
	printf("\t[0-1]   MODULEMODE              : %X\n\n", reg_value & 0x02 );
	
}
//-----------------------------------------------------------------------------------------------
/*********************************
 Enable GPIO (enable clock)
 *******************************
 * Enable GPIO module clock
 *	@param gpio		: GPIO number , BBBIO_GPIO0 / BBBIO_GPIO1 / BBBIO_GPIO2 / BBBIO_GPIO3 .
 *
 *	@return : 1 for success , 0 for failed
 *
 *	@example : BBBIO_sys_Enable_GPIO(BBBIO_GPIO2);	// enable GPIO2's module clock
 *
 *	Warring : please confirm Device which loaded on this GPIO is Disable , or it may cause some bad effect of these device
 */
int BBBIO_sys_Enable_GPIO(unsigned int gpio)		// Enable GPIOx's clock
{
	int param_error=0;				// parameter error 
	volatile unsigned int* reg;		// GPIO register

	// sanity checks
	if (cm_per_addr==0)
		param_error=1;
	if (gpio <0 || gpio >3)	//GPIO range
		param_error=1;

	if (param_error)
	{
		if (BBBIO_LIB_DBG) printf("BBBIO_sys_Enable_GPIO: parameter error!\n");
		return 0;
	}

	switch(gpio)
	{
		case BBBIO_GPIO0 :
		{
			reg =(void*)cm_wkup_addr + BBBIO_CM_WKUP_GPIO0_CLKCTRL;
			*reg |= 0x2 ;
		}
		break ;
		case BBBIO_GPIO1 :
		{
			reg =(void*)cm_per_addr + BBBIO_CM_PER_GPIO1_CLKCTRL;
			*reg |= 0x2 ;
		}
		break ;
		case BBBIO_GPIO2 :
		{
			reg =(void*)cm_per_addr + BBBIO_CM_PER_GPIO2_CLKCTRL;
			*reg |= 0x2 ;
		}
		break ;
		case BBBIO_GPIO3 :
		{
			reg =(void*)cm_per_addr + BBBIO_CM_PER_GPIO3_CLKCTRL;
			*reg |= 0x2 ;
		}
		break ;
		default :
		{
			return 0 ;
		}
		break ;
	}
	return 1 ;
}
//-----------------------------------------------------------------------------------------------
/*********************************
 Disable GPIO (Disable clock)
 *******************************
 * Disable GPIO module clock
 *	@param gpio		: GPIO number , BBBIO_GPIO0 / BBBIO_GPIO1 / BBBIO_GPIO2 / BBBIO_GPIO3 .
 *
 *	@return : 1 for success , 0 for failed
 *
 *	@example : BBBIO_sys_Disable_GPIO(BBBIO_GPIO2);	// Disable GPIO2's module clock
 *
 *	Warring : please confirm Device which loaded on this GPIO is Disable , or it may cause some bad effect of these device
 */
int BBBIO_sys_Disable_GPIO(unsigned int gpio)		// Disable GPIOx's clock
{
	int param_error=0;				// parameter error 
	volatile unsigned int* reg;		// GPIO register

	// sanity checks
	if (cm_per_addr==0)
		param_error=1;
	if (gpio <0 || gpio >3)	//GPIO range
		param_error=1;

	if (param_error)
	{
		if (BBBIO_LIB_DBG) printf("BBBIO_sys_Disable_GPIO: parameter error!\n");
		return 0;
	}

	switch(gpio)
	{
		case BBBIO_GPIO0 :
		{
			reg =(void*)cm_wkup_addr + BBBIO_CM_WKUP_GPIO0_CLKCTRL;
                        *reg &= 0x2 ;
		}
		break ;
		case BBBIO_GPIO1 :
		{
			reg =(void*)cm_per_addr + BBBIO_CM_PER_GPIO1_CLKCTRL;
			*reg &= ~0x2 ;
		}
		break ;
		case BBBIO_GPIO2 :
		{
			reg =(void*)cm_per_addr + BBBIO_CM_PER_GPIO2_CLKCTRL;
			*reg &= ~0x2 ;
		}
		break ;
		case BBBIO_GPIO3 :
		{
			reg =(void*)cm_per_addr + BBBIO_CM_PER_GPIO3_CLKCTRL;
			*reg &= ~0x2 ;
		}
		break ;
		default :
		{
			return 0 ;
		}
		break ;
	}
	return 1 ;
}
//-------------------------------------------------------------------------------------------
/*********************************
  millisecond  sleep
 *******************************
 *      @param msec : sleep time ,millisecond unit .
 *
 *      @return : 1 for success , 0 for failed
 *
 *      @example : BBBIO_sys_delay_ms(2); // Sleep 2ms
 *
 *      Warring : this function is not an accurate function , it's effected by os ,
 *		  so , if you want to take an accurate action  , like motor controller , this function may take some bad effect .
 */

inline int
BBBIO_sys_delay_ms(unsigned long msec)
{
        struct timespec a;

        a.tv_nsec=(msec)*1000000L;
        a.tv_sec=0;
        if (nanosleep(&a, NULL) != 0)
        {
	    fprintf(stderr, "delay_ms error: %s\n", strerror(errno));
	    return 0;
        }
        return 1;
}


//-------------------------------------------------------------------------------------------
/*********************************
 micro second sleep
 *******************************
 *      @param msec : sleep time ,microsecond unit .
 *
 *      @return : 1 for success , 0 for failed
 *
 *      @example : BBBIO_sys_delay_us(2000); // Sleep 2000us (2ms)
 *
 *      Warring : this function is not an accurate function , it's effected by os ,
 *                so , if you want to take an accurate action  , like motor controller , this function may take some bad effect .
 */

inline int
BBBIO_sys_delay_us(unsigned long msec)
{
        struct timespec a;

        a.tv_nsec=(msec)*1000L;
        a.tv_sec=0;
        if (nanosleep(&a, NULL) != 0)
        {
            fprintf(stderr, "delay_ms error: %s\n", strerror(errno));
        }
        return 0;
}

