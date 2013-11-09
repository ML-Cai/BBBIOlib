// BBBiolib.h
// Simple I/O library of Beaglebone balck
// v1 	October 2013 - shabaz (iolib)
// v2 	October 2013 - shabaz (iolib)
// V2.1 November 2013 - VagetableAvenger (BBBlib)

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include "BBBiolib.h"

// Memory mapping offset if GPIO , means the memory address of GPIOs
const unsigned int GPIO_AddressOffset[]={BBBIO_GPIO0, BBBIO_GPIO1, BBBIO_GPIO2, BBBIO_GPIO3};

// GPIO Port number set of Beaglebone Black P8  ,
// -1 as GND or VCC , 0 / 1 / 2 for GPIO 0/1/2
const char p8_PortSet[]={-1,-1, 1, 1, 1, 1, 2, 2,
                         2 , 2, 1, 1, 0, 0, 1, 1,
                         0 , 2, 0, 1, 1, 1, 1, 1,
                         1 , 1, 2, 2, 2, 2, 0, 0,
                         0 , 2, 0, 2, 2, 2, 2,-1,
                         2 , 2, 2, 2, 2, 2};

//  GPIO Port ID set of Beaglebone Black P8  ,
// 0 as GND , offset X as GPIO ID m this value must combine with GPIO number
const unsigned int p8_PortIDSet[]={
					0		,0		,1<<6	,1<<7	,1<<2	,1<<3	,1<<2	,1<<3,
					1<<5	,1<<4	,1<<13	,1<<12	,1<<23	,1<<26	,1<<15	,1<<14,
					1<<27	,1<<1	,1<<22	,1<<31	,1<<30	,1<<5	,1<<4	,1<<1,
					1<<0	,1<<29	,1<<22	,1<<24	,1<<23	,1<<25	,1<<10	,1<<11,
					1<<9	,1<<17	,1<<8	,1<<16	,1<<14	,1<<15	,1<<12	,0,
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
	if (BBBIO_PINMUX_EN)
	{
		ctrl_addr = mmap(0, BBBIO_CONTROL_LEN, PROT_READ | PROT_WRITE, MAP_SHARED, ctrlh, BBBIO_CONTROL_MODULE);
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
	if (ctrlh!=0)
	{
		close(ctrlh);
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
	return ((*((unsigned int *)((void *)gpio_addr[PortSet_ptr[port-8][pin-1]]+BBBIO_GPIO_DATAIN)) & PortIDSet_ptr[port-8][pin-1])!=0);
}
//-----------------------------------------------------------------------------------------------
inline char
is_low(char port, char pin)
{
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


