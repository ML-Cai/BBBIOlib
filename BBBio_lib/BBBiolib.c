/* BBBiolib.c
 * Simple I/O library of Beaglebone balck
 *
 * this fnnction is under construction , so mix some libio and BBBIO express ,
 *     libio : support some basic function for Beaglebone black I/O .
 *     BBBIO : support I/O function using GPIO as unit .
 *
 * all function will integrated as soon as fast .
 *
 * v1 	October 2013 - shabaz (iolib)
 * v2 	October 2013 - shabaz (iolib)
 * v2.1 November 2013 - VagetableAvenger (BBBlib)
 * v2.2 November 2013 - VagetableAvenger (BBBlib) : add GPIO Enable/Disable function
 * v2.4 November.23 2013 - VagetableAvenger (BBBlib) : add whole GPIO control function (I/O and direction set)
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#include "BBBiolib.h"
/*
#include "BBBiolib_PWMSS.h"
#include "BBBiolib_McSPI.h"
#include "BBBiolib_ADCTSC.h"
*/
/* Memory mapping offset if GPIO , means the memory address of GPIOs */
const unsigned int GPIO_AddressOffset[]={BBBIO_GPIO0_ADDR, BBBIO_GPIO1_ADDR, BBBIO_GPIO2_ADDR, BBBIO_GPIO3_ADDR};

/* GPIO Port number set of Beaglebone Black P8 .
 * -1 as GND or VCC , Number 0/1/2 as GPIO 0/1/2
 */
const signed char p8_PortSet[] = {-1, -1, 1, 1, 1, 1, 2, 2,
				2, 2, 1, 1, 0, 0, 1, 1, 
				0, 2, 0, 1, 1, 1, 1, 1, 
				1, 1, 2, 2, 2, 2, 0, 0, 
				0, 2, 0, 2, 2, 2, 2, 2, 
				2, 2, 2, 2, 2, 2};

/* GPIO Port ID set of Beaglebone Black P8  ,
 * 0 as GND , offset X as GPIO ID m this value must combine with GPIO number
 */
const unsigned int p8_PortIDSet[] = {0,	0,	1<<6,	1<<7,	1<<2,	1<<3,	1<<2,	1<<3,	
				1<<5,	1<<4,	1<<13,	1<<12,	1<<23,	1<<26,	1<<15,	
				1<<14,	1<<27,	1<<1,	1<<22,	1<<31,	1<<30,	1<<5,	
				1<<4,	1<<1,	1<<0,	1<<29,	1<<22,	1<<24,	1<<23,	
				1<<25,	1<<10,	1<<11,	1<<9,	1<<17,	1<<8,	1<<16,	
				1<<14,	1<<15,	1<<12,	1<<13,	1<<10,	1<<11,	1<<8,	
				1<<9,	1<<6,	1<<7};

/* GPIO Port number set of Beaglebone Black P9  ,
 * -1 as GND or VCC , 0 / 1 / 2 for GPIO 0/1/2
 */
const signed char p9_PortSet[] = {-1, -1, -1, -1, -1, -1, -1,
				-1, -1, -1, 0, 1, 0, 1, 1, 
				1, 0, 0, 0, 0, 0, 0, 1, 0, 
				3, 0, 3, 3, 3, 3, 3, -1, -1, 
				-1, -1, -1, -1, -1, -1, -1, 
				0, 0, -1, -1, -1, -1};

/*  GPIO Port ID set of Beaglebone Black P9  ,
 * 0 as GND , offset X as GPIO ID m this value must combine with GPIO number
 */
const unsigned int p9_PortIDSet[]={0,	0,	0,	0,	0,	0,	0,	0,	
				0,	0,	1<<30,	1<<28,	1<<31,	1<<18,	1<<16,	1<<19,	
				1<<5,	1<<4,	1<<13,	1<<12,	1<<3,	1<<2,	1<<17,	
				1<<15,	1<<21,	1<<14,	1<<19,	1<<17,	1<<15,	1<<16,	
				1<<14,	0,	0,	0,	0,	0,	0,	0,	0,	0,	1<<20,	
				1<<7,	0,	0,	0,	0};

/* Memory Handle and Control Handle */
int memh=0;
int ctrlh=0;
volatile unsigned int *gpio_addr[4] = {NULL, NULL, NULL, NULL};
volatile unsigned int *CM_ptr = NULL;
volatile unsigned int *cm_per_addr = NULL;
volatile unsigned int *cm_wkup_addr = NULL ;

/* pointer to const Port set and Port ID set array */
signed char* PortSet_ptr[2];
unsigned int* PortIDSet_ptr[2];
/*-----------------------------------------------------------------------------------------------
 * ********************************
 * Library Init
 * ********************************
*/
int iolib_init(void)
{
	int i;

	if (memh) {
#ifdef BBBIO_LIB_DBG
		printf("iolib_init: memory already mapped?\n");
#endif
		return -1;
	}

	PortSet_ptr[0]=(char*)p8_PortSet;
	PortSet_ptr[1]=(char*)p9_PortSet;
	PortIDSet_ptr[0]=(unsigned int*)p8_PortIDSet;
	PortIDSet_ptr[1]=(unsigned int*)p9_PortIDSet;

	/* using memory mapped I/O */
	memh=open("/dev/mem", O_RDWR);

	/* mapping Clock Module Peripheral Registers */
	cm_per_addr = mmap(0, BBBIO_CM_PER_LEN, PROT_READ | PROT_WRITE, MAP_SHARED, memh, BBBIO_CM_PER_ADDR);
	if(cm_per_addr == MAP_FAILED)
    	{
#ifdef BBBIO_LIB_DBG
		printf("iolib_init: CM_PER mmap failure! ,error :%s\n" ,strerror(errno));
#endif
		return -1;
	}

	/* mapping Clock Module Wakeup Registers
	 *
	 *  Note : the reason why not using mmap to mapping cm_wkup direct is page boundar .
	 * 	   if using mmap directlly (like CM_PER ), it will cause an error about EINVAL (invalid argument) .
	 *	   because the address of CM_WKUP is 0x44E00400 , it doesn't align the page (4K).
	 */
	cm_wkup_addr =(void *)cm_per_addr + BBBIO_CM_WKUP_OFFSET_FROM_CM_PER ;


	/* mapping Address of GPIO 0~4 */
	for (i=0; i<4; i++) {
		gpio_addr[i] = mmap(0 ,BBBIO_GPIOX_LEN ,PROT_READ | PROT_WRITE ,MAP_SHARED ,memh ,GPIO_AddressOffset[i]);
		if(gpio_addr[i] == MAP_FAILED) {
#ifdef BBBIO_LIB_DBG
			printf("iolib_init: gpio mmap failure!\n");
#endif
			return -1;
		}
	}

	/* mapping Control Module Registers
	 * for pin mux control , or display expansion header informaiton
	 *
	 * Useless now , this register must be privigle mode .
	 */
	CM_ptr = mmap(0, BBBIO_CONTROL_LEN, PROT_READ | PROT_WRITE, MAP_SHARED, memh, BBBIO_CONTROL_MODULE);
	if(CM_ptr == MAP_FAILED) {
#ifdef BBBIO_LIB_DBG
		printf("iolib_init: control module mmap failure!\n");
#endif
		return -1;
	}

	BBBIO_PWM_Init();
	BBBIO_McSPI_Init();
	BBBIO_ADCTSC_Init();

	return 0;
}
/*-----------------------------------------------------------------------------------------------
 * ********************************
 * Library free
 * ********************************
 */
int iolib_free(void)
{
	if (memh!=0) {
		close(memh);
	}
	return 0;
}
/*-----------------------------------------------------------------------------------------------
 * ********************************
 * Set I/O direction (Input/Output)
 * ********************************
 */
int iolib_setdir(char port, char pin, char dir)
{
	int param_error=0;			// parameter error
	volatile unsigned int* reg;		// GPIO register

	// sanity checks
	if (memh == 0)
		param_error=1;
	if ((port < 8) || (port > 9))		// if input is not port8 and port 9 , because BBB support P8/P9 Connector
		param_error=1;
	if ((pin < 1 ) || (pin > 46))		// if pin over/underflow , range : 1~46
		param_error=1;
	if (PortSet_ptr[port - 8][pin - 1] < 0)	// pass GND OR VCC (PortSet as -1)
		param_error=1;

	if (param_error)
	{
#ifdef BBBIO_LIB_DBG
		printf("iolib_setdir: parameter error!\n");
#endif
		return(-1);
	}
#ifdef BBBIO_LIB_DBG
	printf("iolib_setdir: PortSet_ptr P%d.%d , %X\n",port ,pin , PortSet_ptr[port-8][pin-1]);
#endif
	reg=(void*)gpio_addr[PortSet_ptr[port-8][pin-1]] +BBBIO_GPIO_OE;

	if (dir == BBBIO_DIR_OUT) {
		*reg &= ~(PortIDSet_ptr[port-8][pin-1]);
	}
	else if (dir == BBBIO_DIR_IN) {
		*reg |= PortIDSet_ptr[port-8][pin-1];
	}

	return(0);
}
/* ----------------------------------------------------------------------------------------------- */
void pin_high(char port, char pin)
{
	*((unsigned int *)((void *)gpio_addr[PortSet_ptr[port-8][pin-1]]+BBBIO_GPIO_SETDATAOUT)) = PortIDSet_ptr[port-8][pin-1];
}
/* ----------------------------------------------------------------------------------------------- */
void pin_low(char port, char pin)
{
	*((unsigned int *)((void *)gpio_addr[PortSet_ptr[port-8][pin-1]]+BBBIO_GPIO_CLEARDATAOUT)) = PortIDSet_ptr[port-8][pin-1];
}
/* ----------------------------------------------------------------------------------------------- */
char is_high(char port, char pin)
{
	return ((*((unsigned int *)((void *)gpio_addr[PortSet_ptr[port-8][pin-1]]+BBBIO_GPIO_DATAIN)) & PortIDSet_ptr[port-8][pin-1])!=0);
}
/* ----------------------------------------------------------------------------------------------- */
char is_low(char port, char pin)
{
	return ((*((unsigned int *)((void *)gpio_addr[PortSet_ptr[port-8][pin-1]]+BBBIO_GPIO_DATAIN)) & PortIDSet_ptr[port-8][pin-1])==0);
}
/* ----------------------------------------------------------------------------------------------- */
int iolib_delay_ms(unsigned int msec)
{
	int ret;
	struct timespec a;
	if (msec > 999) {
		fprintf(stderr, "delay_ms error: delay value needs to be less than 999\n");
		msec = 999;
	}
	a.tv_nsec = ((long)(msec)) * 1000000;
	a.tv_sec = 0;
	if ((ret = nanosleep(&a, NULL)) != 0) {
		fprintf(stderr, "delay_ms error: %s\n", strerror(errno));
	}
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
void BBBIO_sys_GPIO_CLK_status()
{
	volatile unsigned int* reg ;
	unsigned int reg_value =0;
	const char s_GDBLCK[][9]={"FCLK_DIS" ,"FCLK_EN"};
	const char s_IDLEST[][8]={"Func" ,"Trans" ,"Idle" ,"Disable"};
	const char s_MODULEMODE[][8]={"Disable" ,"Reserve" ,"Enable" ,"Reserve"};

	int v_GDBLCK ;
	int v_IDLEST ;
	int v_MODULEMODE ;

	printf("\n******************************************************\n");
	printf("************ GPIO Clock module Information ***********\n");
	printf("******************************************************\n");
	reg =(void*)cm_per_addr + BBBIO_CM_PER_L4LS_CLKSTCTRL;
	reg_value = *reg ;
	printf("CM_PER CM_PER_L4LS_CLKSTCTRL : %X\n\n", reg_value );

	reg =(void*)cm_wkup_addr + BBBIO_CM_WKUP_GPIO0_CLKCTRL ;
        reg_value = *reg ;
	v_GDBLCK = (reg_value >> 18) & 0x01 ;
	v_IDLEST = (reg_value >> 16) & 0x02 ;
	v_MODULEMODE =  reg_value & 0x02 ;

        printf("CM_WKUP CM_WKUP_GPIO0_CLKCTRL : %X\n", *reg );
        printf("\t[18]    OPTFCLKEN_GPIO_0_GDBCLK : %X (%s)\n", v_GDBLCK ,s_GDBLCK[v_GDBLCK]);
        printf("\t[17-18] IDLEST                  : %X (%s)\n", v_IDLEST ,s_IDLEST[v_IDLEST]);
        printf("\t[0-1]   MODULEMODE              : %X (%s)\n\n", v_MODULEMODE ,s_MODULEMODE[v_MODULEMODE]);

	reg =(void*)cm_per_addr + BBBIO_CM_PER_GPIO1_CLKCTRL;
	reg_value = *reg ;
        v_GDBLCK = (reg_value >> 18) & 0x01 ;
        v_IDLEST = (reg_value >> 16) & 0x02 ;
        v_MODULEMODE =  reg_value & 0x02 ;

	printf("CM_PER CM_PER_GPIO1_CLKCTRL : %X\n", *reg );
        printf("\t[18]    OPTFCLKEN_GPIO_1_GDBCLK : %X (%s)\n", v_GDBLCK ,s_GDBLCK[v_GDBLCK]);
        printf("\t[17-18] IDLEST                  : %X (%s)\n", v_IDLEST ,s_IDLEST[v_IDLEST]);
        printf("\t[0-1]   MODULEMODE              : %X (%s)\n\n", v_MODULEMODE ,s_MODULEMODE[v_MODULEMODE]);

	reg =(void*)cm_per_addr + BBBIO_CM_PER_GPIO2_CLKCTRL;
	reg_value = *reg ;
        v_GDBLCK = (reg_value >>18) & 0x01 ;
        v_IDLEST = (reg_value >>16) & 0x02 ;
        v_MODULEMODE =  reg_value & 0x02 ;

	printf("CM_PER CM_PER_GPIO2_CLKCTRL : %X\n", *reg );
        printf("\t[18]    OPTFCLKEN_GPIO_2_GDBCLK : %X (%s)\n", v_GDBLCK ,s_GDBLCK[v_GDBLCK]);
        printf("\t[17-18] IDLEST                  : %X (%s)\n", v_IDLEST ,s_IDLEST[v_IDLEST]);
        printf("\t[0-1]   MODULEMODE              : %X (%s)\n\n", v_MODULEMODE ,s_MODULEMODE[v_MODULEMODE]);

	reg =(void*)cm_per_addr + BBBIO_CM_PER_GPIO3_CLKCTRL;
	reg_value = *reg ;
        v_GDBLCK = (reg_value >> 18) & 0x01 ;
        v_IDLEST = (reg_value >> 16) & 0x02 ;
        v_MODULEMODE =  reg_value & 0x02 ;

	printf("CM_PER CM_PER_GPIO3_CLKCTRL : %X\n", *reg );
        printf("\t[18]    OPTFCLKEN_GPIO_0_GDBCLK : %X (%s)\n", v_GDBLCK ,s_GDBLCK[v_GDBLCK]);
        printf("\t[17-18] IDLEST                  : %X (%s)\n", v_IDLEST ,s_IDLEST[v_IDLEST]);
        printf("\t[0-1]   MODULEMODE              : %X (%s)\n\n", v_MODULEMODE ,s_MODULEMODE[v_MODULEMODE]);
}
//-----------------------------------------------------------------------------------------------
/**************************************************
 Show Beaglebone Black Expansion Header Information
 **************************************************
 *
 *      @param gpio     : GPIO number , BBBIO_GPIO0 / BBBIO_GPIO1 / BBBIO_GPIO2 / BBBIO_GPIO3 .
 *
 *
 *      @example        : BBBIO_sys_Expansion_Header_status(BBBIO_GPIO1);
 *
 */

const unsigned int ExpHeader_MODE0_P8[]={BBBIO_EXPANSION_HEADER_GND ,    // 1
					BBBIO_EXPANSION_HEADER_GND ,    // 2
					BBBIO_CONF_GPMC_AD6 ,           // 3
					BBBIO_CONF_GPMC_AD7 ,           // 4
					BBBIO_CONF_GPMC_AD2 ,           // 5
					BBBIO_CONF_GPMC_AD3 ,           // 6
					BBBIO_CONF_GPMC_ADVN_ALE ,      // 7
					BBBIO_CONF_GPMC_OEN_REN ,       // 8
					BBBIO_CONF_GPMC_BEN0_CLE ,      // 9
					BBBIO_CONF_GPMC_WEN ,           // 10
					BBBIO_CONF_GPMC_AD13 ,          // 11
					BBBIO_CONF_GPMC_AD12 ,          // 12
					BBBIO_CONF_GPMC_AD9 ,           // 13
					BBBIO_CONF_GPMC_AD10 ,          // 14
					BBBIO_CONF_GPMC_AD15 ,          // 15
					BBBIO_CONF_GPMC_AD14 ,          // 16
					BBBIO_CONF_GPMC_AD11 ,          // 17

					BBBIO_EXPANSION_HEADER_UNKNOW ,
//					BBBIO_CONF_GPMC_CLK_MUX0 ,      // 18

					BBBIO_CONF_GPMC_AD8 ,           // 19
					BBBIO_CONF_GPMC_CSN2 ,          // 20
					BBBIO_CONF_GPMC_CSN1 ,          // 21
					BBBIO_CONF_GPMC_AD5 ,           // 22
					BBBIO_CONF_GPMC_AD4 ,           // 23
					BBBIO_CONF_GPMC_AD1 ,           // 24
					BBBIO_CONF_GPMC_AD0 ,           // 25
					BBBIO_CONF_GPMC_CSN0 ,          // 26
					BBBIO_CONF_LCD_VSYNC ,          // 27
					BBBIO_CONF_LCD_PCLK ,           // 28
					BBBIO_CONF_LCD_HSYNC ,          // 29
					BBBIO_CONF_LCD_AC_BIAS_EN ,     // 30
					BBBIO_CONF_LCD_DATA14 ,         // 31
					BBBIO_CONF_LCD_DATA15 ,         // 32
					BBBIO_CONF_LCD_DATA13 ,         // 33
					BBBIO_CONF_LCD_DATA11 ,         // 34
					BBBIO_CONF_LCD_DATA12 ,         // 35
					BBBIO_CONF_LCD_DATA10 ,         // 36
					BBBIO_CONF_LCD_DATA8 ,          // 37
					BBBIO_CONF_LCD_DATA9 ,          // 38
					BBBIO_CONF_LCD_DATA6 ,          // 39
					BBBIO_CONF_LCD_DATA7 ,          // 40
					BBBIO_CONF_LCD_DATA4 ,          // 41
					BBBIO_CONF_LCD_DATA5 ,          // 42
					BBBIO_CONF_LCD_DATA2 ,          // 43
					BBBIO_CONF_LCD_DATA3 ,          // 44
					BBBIO_CONF_LCD_DATA0 ,          // 45
					BBBIO_CONF_LCD_DATA1 };         // 46


const unsigned int ExpHeader_MODE0_P9[]={BBBIO_EXPANSION_HEADER_GND ,		// 1
					BBBIO_EXPANSION_HEADER_GND ,		// 2
					BBBIO_EXPANSION_HEADER_DC_33V ,		// 3
					BBBIO_EXPANSION_HEADER_DC_33V ,		// 4
					BBBIO_EXPANSION_HEADER_VDD_5V ,		// 5
					BBBIO_EXPANSION_HEADER_VDD_5V ,		// 6
					BBBIO_EXPANSION_HEADER_SYS_5V ,		// 7
					BBBIO_EXPANSION_HEADER_SYS_5V ,		// 8
					BBBIO_EXPANSION_HEADER_PWR_BUT ,	// 9
					BBBIO_EXPANSION_HEADER_SYS_RESETN ,	// 10
					BBBIO_CONF_GPMC_WAIT0 ,				// 11

					BBBIO_EXPANSION_HEADER_UNKNOW ,
//					BBBIO_CONF_GPMC_BE1N ,				// 12

					BBBIO_CONF_GPMC_WPN ,				// 13
					BBBIO_CONF_GPMC_A2 ,				// 14
					BBBIO_CONF_GPMC_A0 ,				// 15
					BBBIO_CONF_GPMC_A3 ,				// 16
					BBBIO_CONF_SPI0_CS0 ,				// 17
					BBBIO_CONF_SPI0_D1 ,				// 18
					BBBIO_CONF_UART1_RTSN ,				// 19
					BBBIO_CONF_UART1_CTSN ,				// 20
					BBBIO_CONF_SPI0_D0 ,				// 21
					BBBIO_CONF_SPI0_SCLK ,				// 22
					BBBIO_CONF_GPMC_A1 ,				// 23
					BBBIO_CONF_UART1_TXD ,				// 24
					BBBIO_CONF_MCASP0_AHCLKX ,			// 25
					BBBIO_CONF_UART1_RXD ,				// 26
					BBBIO_CONF_MCASP0_FSR ,				// 27
					BBBIO_CONF_MCASP0_AHCLKR ,			// 28
					BBBIO_CONF_MCASP0_FSX ,				// 29
					BBBIO_CONF_MCASP0_AXR0 ,			// 30
					BBBIO_CONF_MCASP0_ACLKX ,			// 31
					BBBIO_EXPANSION_HEADER_VADC ,		// 32
					BBBIO_EXPANSION_HEADER_AIN4 ,		// 33
					BBBIO_EXPANSION_HEADER_AGND ,		// 34
					BBBIO_EXPANSION_HEADER_AIN6 ,		// 35
					BBBIO_EXPANSION_HEADER_AIN5 ,		// 36
					BBBIO_EXPANSION_HEADER_AIN2 ,		// 37
					BBBIO_EXPANSION_HEADER_AIN3 ,		// 38
					BBBIO_EXPANSION_HEADER_AIN0 ,		// 39
					BBBIO_EXPANSION_HEADER_AIN1 ,		// 40
					BBBIO_CONF_XDMA_EVENT_INTR1 ,		// 41
					BBBIO_CONF_ECAP0_IN_PWM0_OUT ,		// 42
					BBBIO_EXPANSION_HEADER_GND ,		// 43
					BBBIO_EXPANSION_HEADER_GND ,		// 44
					BBBIO_EXPANSION_HEADER_GND ,            // 45
                                        BBBIO_EXPANSION_HEADER_GND };           // 46

/* Expansion Header access ptr */
const unsigned int* ExpHeader_MODE0[2]={ExpHeader_MODE0_P8,ExpHeader_MODE0_P9};

void BBBIO_sys_Expansion_Header_status(unsigned int port)
{
        volatile unsigned int* reg ;
        unsigned int reg_value =0;
	int i ;

	/*register value express  string */
	const char extra_status[][12]={ "GND" ,"DC_33V" ,"CDD_5V" ,"SYS_5V" ,"PWR_BUT" ,
				       	"SYS_RESET" ,"VADC" ,"AIN4" ,"AGND" ,"AIN6" ,"AIN5" ,
				       	"AIN2" ,"AIN3" ,"AIN0" ,"AIN1" ,"Unknow"};
	const char s_SLEWCTRL[2][5] ={"Fast" ,"Slow"} ;
	const char s_EXACTIVE[2][8] ={"Disable" ,"Enable"} ;
	const char s_PULLTYPESEL[2][9]={"Pulldown" ,"Pullup"};
	const char s_PULLUPEN[2][8] ={"Enable" ,"Disable"} ;

	int v_SLEWCTRL ;	//register value
	int v_EXACTIVE ;
	int v_PULLTYPESEL ;
	int v_PULLUPEN ;
	int v_MODE ;

	if ((port<8) || (port>9))               // if input is not port8 and port 9 , because BBB support P8/P9 Connector
	{
	    printf("Expansion_Header only have P8 and P9 ,please check your input port :%d\n",port);
            return ;
	}
	port -=8 ;

        printf("\n******************************************************\n");
        printf("************ Expansion Header Information ************\n");
        printf("******************************************************\n");

	for(i=0 ;i< 46 ; i++)
 	{
	    if(ExpHeader_MODE0[port][i] &0xF0000000)	// VCC /GND .........
	    {
		printf("P%d_%2d : \t%s\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t| P%d_%2d\n" ,port+8 ,i+1 ,
							&extra_status[ExpHeader_MODE0[port][i] &0x0f][0] ,
							port+8 ,i+1);
	    }
	    else					// functional pins
	    {
	    	reg =(void*)CM_ptr + ExpHeader_MODE0[port][i] ;
            	reg_value = *reg ;

		v_SLEWCTRL = reg_value>>6 ;
		v_EXACTIVE = reg_value >>5 &0x01;
		v_PULLTYPESEL = reg_value >>4 & 0x01;
		v_PULLUPEN = reg_value >>3 & 0x01;
		v_MODE	=reg_value  &0x07;

		printf("P%d_%2d : \tSLEWCTRL %d (%s) \t,RXACTIVE %d (%s)\t,PULLTYPESEL %d (%s) \t,PULLDEN %d (%s) \t,MUXMODE : %d \t| P%d_%2d \n" ,
				port+8 ,i+1 ,
				v_SLEWCTRL ,s_SLEWCTRL[v_SLEWCTRL] ,
				v_EXACTIVE ,s_EXACTIVE[v_EXACTIVE] ,
				v_PULLTYPESEL ,s_PULLTYPESEL[v_PULLTYPESEL] ,
				v_PULLUPEN ,s_PULLUPEN[v_PULLUPEN] ,
				v_MODE ,
				port+8 ,i+1);
	    }
	}
}
//-----------------------------------------------------------------------------------------------
// *********************************
// no effect , don't use that!!
// ********************************
/*
#define BBBIO_PINMUX_SLEWCTRL	(1<<6)
#define BBBIO_PINMUX_FAST_RATE	(BBBIO_PINMUX_SLEWCTRL | (1<<6)<<8)
#define BBBIO_PINMUX_SLOW_RATE	(BBBIO_PINMUX_SLEWCTRL | (0<<6)<<8)

#define BBBIO_PINMUX_RXACTIVE	(1<<5)
#define BBBIO_PINMUX_RX_DISABLE	(BBBIO_PINMUX_RXACTIVE | (1<<5)<<8)
#define BBBIO_PINMUX_RX_ENABLE	(BBBIO_PINMUX_RXACTIVE | (0<<5)<<8)

#define BBBIO_PINMUX_PUTYPESEL	(1<<4)
#define BBBIO_PINMUX_PULLUP	(BBBIO_PINMUX_PUTYPESEL | (1<<4)<<8)
#define BBBIO_PINMUX_PULLDOWN	(BBBIO_PINMUX_PUTYPESEL | (0<<4)<<8)

#define BBBIO_PINMUX_PUDEN	(1<<3)
#define BBBIO_PINMUX_PULL_ENABLE	(BBBIO_PINMUX_PUDEN | (1<<3)<<8)
#define BBBIO_PINMUX_PULL_DISABLE	(BBBIO_PINMUX_PUDEN | (0<<3)<<8)

#define BBBIO_PINMUX_MODE	(0x7)
#define BBBIO_PINMUX_MODE_0	(BBBIO_PINMUX_MODE | (0x0)<<8)
#define BBBIO_PINMUX_MODE_1	(BBBIO_PINMUX_MODE | (0x1)<<8)
#define BBBIO_PINMUX_MODE_2	(BBBIO_PINMUX_MODE | (0x2)<<8)
#define BBBIO_PINMUX_MODE_3	(BBBIO_PINMUX_MODE | (0x3)<<8)
#define BBBIO_PINMUX_MODE_4	(BBBIO_PINMUX_MODE | (0x4)<<8)
#define BBBIO_PINMUX_MODE_5	(BBBIO_PINMUX_MODE | (0x5)<<8)
#define BBBIO_PINMUX_MODE_6	(BBBIO_PINMUX_MODE | (0x6)<<8)
#define BBBIO_PINMUX_MODE_7	(BBBIO_PINMUX_MODE | (0x7)<<8)
*/

int BBBIO_sys_pinmux_check(unsigned int port, unsigned int pin, unsigned int Cflag )
{
	volatile unsigned int* reg;
	unsigned int reg_value = 0;
	unsigned int ret = 0;
	unsigned int reg_tmp ;
	unsigned int Cflag_tmp ;

	// sanity checks
	if (memh == 0)
		goto PARAM_ERROR ;
	if ((port < 8) || (port > 9))               // if input is not port8 and port 9 , because BBB support P8/P9 Connector
		goto PARAM_ERROR ;
	if ((pin < 1) || (pin > 46))                // if pin over/underflow , range : 1~46
		goto PARAM_ERROR ;
	if (PortSet_ptr[port - 8][pin - 1] < 0)   	// pass GND OR VCC (PortSet as -1)
		goto PARAM_ERROR ;

	port -= 8;
	pin -= 1;

	reg =(void *)CM_ptr + ExpHeader_MODE0[port][pin] ;
	reg_value = *reg ;

	if(Cflag & BBBIO_PINMUX_SLEWCTRL && (ret == 0)) {
		reg_tmp = reg_value & BBBIO_PINMUX_SLEWCTRL;
		Cflag_tmp = (Cflag >> 8) & BBBIO_PINMUX_SLEWCTRL;
		ret = reg_tmp ^ Cflag_tmp;
	}
	if((Cflag & BBBIO_PINMUX_RXACTIVE) && (ret == 0)) {
		reg_tmp = reg_value & BBBIO_PINMUX_RXACTIVE;
		Cflag_tmp = (Cflag >> 8) & BBBIO_PINMUX_RXACTIVE;
		ret = reg_tmp ^ Cflag_tmp;
	}

	if((Cflag & BBBIO_PINMUX_PUTYPESEL) && (ret == 0)) {
		reg_tmp = reg_value & BBBIO_PINMUX_PUTYPESEL;
		Cflag_tmp = (Cflag >> 8) & BBBIO_PINMUX_PUTYPESEL;
		ret = reg_tmp ^ Cflag_tmp;
	}

	if((Cflag & BBBIO_PINMUX_PUDEN) && (ret == 0)) {
		reg_tmp = reg_value & BBBIO_PINMUX_PUDEN;
		Cflag_tmp = (Cflag >> 8) & BBBIO_PINMUX_PUDEN;
		ret = reg_tmp ^ Cflag_tmp;
	}

	if((Cflag & BBBIO_PINMUX_MODE) && (ret == 0)) {
		reg_tmp = reg_value & BBBIO_PINMUX_MODE;
		Cflag_tmp = (Cflag >> 8) & BBBIO_PINMUX_MODE;
		ret = reg_tmp ^ Cflag_tmp ;
	}

	return !ret;

PARAM_ERROR :
#ifdef BBBIO_LIB_DBG
	printf("BBBIO_sys_pin_mux_check : parameter error!\n");
#endif
	return 0;
}

//-----------------------------------------------------------------------------------------------
/*********************************
 Enable GPIO Debouncing
 *******************************
 *      @param port     : BBB Expansion Header ID , 8 or 9 .
 *      @param pin    	: which pin yo want to control .
 *      @param DB_time  : Debouncing time , Debouncing Value = (GDB_time + 1) * 31 microseconds
 *
 *      @return         : 0 for success , -1 for failed
 *
 *      @example        : BBBIO_sys_Enable_Debouncing(8 ,11 , 2);	// Enable Expansion Header 8 ,pin 11 's dedbouncing .
 *									// debouncing time is 93 us
 *
 *      Warring         : 1. please check your contorl pin is setted in input mode first , or it may cause some problem .
 *			  2. this function will enable GPIO's GDBCLK automatically .
 */

int  BBBIO_sys_Enable_Debouncing(unsigned int port ,unsigned int pin ,unsigned int GDB_time)
{
	volatile unsigned int* reg;
	int param_error=0;                              // parameter error
	const unsigned int GPIO_CLKCTRL[] ={BBBIO_CM_PER_GPIO1_CLKCTRL ,
					    BBBIO_CM_PER_GPIO2_CLKCTRL ,
					    BBBIO_CM_PER_GPIO3_CLKCTRL};

        // sanity checks
        if (memh==0)
            param_error=1;
        if ((port<8) || (port>9))               // if input is not port8 and port 9 , because BBB support P8/P9 Connector
            param_error=1;
        if ((pin<1) || (pin>46))                // if pin over/underflow , range : 1~46
            param_error=1;
        if (PortSet_ptr[port - 8][pin - 1]<0)   // pass GND OR VCC (PortSet as -1)
            param_error=1;
	if(GDB_time >255)
	    param_error=1;

        if (param_error) {
#ifdef BBBIO_LIB_DBG
		printf("BBBIO_sys_Enable_Debouncing : parameter error!\n");
#endif
		return -1 ;
	}
	port -= 8;
	pin -= 1;

        /* Enable GPIO1 GDBCLK */
	if(PortSet_ptr[port][pin]==0) {	/* CLKCTRL of  GPIO 0 is in CM_WKUP register */
	    reg =(void *)cm_wkup_addr + BBBIO_CM_WKUP_GPIO0_CLKCTRL;
            *reg |= 1 << 18;
	}
	else {
	    reg =(void *)cm_per_addr + GPIO_CLKCTRL[PortSet_ptr[port][pin] - 1] ;
	    *reg |= 1 << 18;
	}

	// Enable Debouncing
        reg = (void *)gpio_addr[PortSet_ptr[port][pin]] + BBBIO_GPIO_DEBOUNCENABLE ;
	*reg |= PortIDSet_ptr[port][pin] ;

	reg = (void *)gpio_addr[PortSet_ptr[port][pin]] + BBBIO_GPIO_DEBOUNCINGTIME ;
	*reg = GDB_time ;

	return 0;

}
//-----------------------------------------------------------------------------------------------
/*********************************
 Disable GPIO Debouncing
 *******************************
 *      @param port     : BBB Expansion Header ID , 8 or 9 .
 *      @param pin      : which pin yo want to control .
 *      @param DB_time  : Debouncing time , Debouncing Value = (GDB_time + 1) * 31 microseconds
 *
 *      @return         : 0 for success , -1 for failed
 *
 *      @example        : BBBIO_sys_Disable_Debouncing(8 ,11 ,0);       // Disable Expansion Header 8 ,pin 11 's dedbouncing .
 *                                                                      // debouncing time is 1 us (no effect , just recover to reset value)
 *
 *      Warring         : 1. please check your contorl pin is setted in input mode first , or it may cause some problem .
 *                         2. this function will enable GPIO's GDBCLK automatically .
 */

int  BBBIO_sys_Disable_Debouncing(unsigned int port ,unsigned int pin ,unsigned int GDB_time)
{
        volatile unsigned int* reg;
        int param_error=0;
        const unsigned int GPIO_CLKCTRL[] ={BBBIO_CM_PER_GPIO1_CLKCTRL ,
                                            BBBIO_CM_PER_GPIO2_CLKCTRL ,
                                            BBBIO_CM_PER_GPIO3_CLKCTRL};

        // sanity checks
        if (memh == 0)
            param_error = 1;
        if ((port < 8) || (port > 9))	/* if input is not port8 and port 9 , because BBB support P8/P9 Connector */
            param_error = 1;
        if ((pin < 1) || (pin > 46))	/* if pin over/underflow , range : 1~46 */
            param_error = 1;
        if (PortSet_ptr[port - 8][pin - 1] < 0)   /* pass GND OR VCC (PortSet as -1) */
            param_error = 1;
        if(GDB_time > 255)
            param_error = 1;

        if (param_error) {
#ifdef BBBIO_LIB_DBG
		printf("BBBIO_sys_Disable_Debouncing : parameter error!\n");
#endif
		return -1 ;
        }
        port -= 8;
        pin -= 1;

	/* Disable Debouncing */
	reg = (void*)gpio_addr[PortSet_ptr[port][pin]] +BBBIO_GPIO_DEBOUNCENABLE ;
	*reg &= ~PortIDSet_ptr[port][pin] ;

	return 0;
}

//-----------------------------------------------------------------------------------------------
/*********************************
 Enable GPIO (enable clock)
 *******************************
 * Enable GPIO module clock
 *	@param gpio	: GPIO number , BBBIO_GPIO0 / BBBIO_GPIO1 / BBBIO_GPIO2 / BBBIO_GPIO3 .
 *
 *	@return		: 1 for success , 0 for failed
 *
 *	@example	: BBBIO_sys_Enable_GPIO(BBBIO_GPIO2);	// enable GPIO2's module clock
 *
 *	Warring		: please confirm Device which loaded on this GPIO is Disable , or it may cause some bad effect of these device
 */
int BBBIO_sys_Enable_GPIO(unsigned int gpio)		// Enable GPIOx's clock
{
	int param_error=0;				// parameter error 
	volatile unsigned int* reg;		// GPIO register

	// sanity checks
	if (cm_per_addr==0)
		param_error=1;
	if (gpio >3)	//GPIO range
		param_error=1;

	if (param_error) {
#ifdef BBBIO_LIB_DBG
		printf("BBBIO_sys_Enable_GPIO: parameter error!\n");
		return 0;
#endif
	}

	switch(gpio) {
	case BBBIO_GPIO0 :
		reg =(void *)cm_wkup_addr + BBBIO_CM_WKUP_GPIO0_CLKCTRL;
		*reg |= 0x2 ;
		break ;
	case BBBIO_GPIO1 :
		reg =(void *)cm_per_addr + BBBIO_CM_PER_GPIO1_CLKCTRL;
		*reg |= 0x2 ;
		break ;
	case BBBIO_GPIO2 :
		reg =(void *)cm_per_addr + BBBIO_CM_PER_GPIO2_CLKCTRL;
		*reg |= 0x2 ;
		break ;
	case BBBIO_GPIO3 :
		reg =(void *)cm_per_addr + BBBIO_CM_PER_GPIO3_CLKCTRL;
		*reg |= 0x2 ;
		break ;
	default :
		return 0 ;
		break ;
	}
	return 1 ;
}
//-----------------------------------------------------------------------------------------------
/*********************************
 Disable GPIO (Disable clock)
 *******************************
 * Disable GPIO module clock
 *	@param gpio	: GPIO number , BBBIO_GPIO0 / BBBIO_GPIO1 / BBBIO_GPIO2 / BBBIO_GPIO3 .
 *
 *	@return 	: 1 for success , 0 for failed
 *
 *	@example 	: BBBIO_sys_Disable_GPIO(BBBIO_GPIO2);	// Disable GPIO2's module clock
 *
 *	Warring 	: please confirm Device which loaded on this GPIO is Disable , or it may cause some bad effect of these device
 */
int BBBIO_sys_Disable_GPIO(unsigned int gpio)		// Disable GPIOx's clock
{
	int param_error=0;				// parameter error
	volatile unsigned int* reg;		// GPIO register

	// sanity checks
	if (cm_per_addr==0)
		param_error=1;
	if (gpio >3)	//GPIO range
		param_error=1;

	if (param_error) {
#ifdef BBBIO_LIB_DBG
		printf("BBBIO_sys_Disable_GPIO: parameter error!\n");
#endif
		return 0;
	}

	switch(gpio) {
	case BBBIO_GPIO0 :
		reg =(void *)cm_wkup_addr + BBBIO_CM_WKUP_GPIO0_CLKCTRL;
		*reg &= 0x2 ;
		break ;
	case BBBIO_GPIO1 :
		reg =(void *)cm_per_addr + BBBIO_CM_PER_GPIO1_CLKCTRL;
		*reg &= ~0x2 ;
		break ;
	case BBBIO_GPIO2 :
		reg =(void *)cm_per_addr + BBBIO_CM_PER_GPIO2_CLKCTRL;
		*reg &= ~0x2 ;
		break ;
	case BBBIO_GPIO3 :
		reg =(void *)cm_per_addr + BBBIO_CM_PER_GPIO3_CLKCTRL;
		*reg &= ~0x2 ;
		break ;
	default :
		return 0 ;
		break ;
	}
	return 1 ;
}
//-------------------------------------------------------------------------------------------
/*********************************
  millisecond  sleep
 *******************************
 *      @param msec	: sleep time ,millisecond unit .
 *
 *      @return 	: 1 for success , 0 for failed
 *
 *      @example 	: BBBIO_sys_delay_ms(2); // Sleep 2ms
 *
 *      Warring 	: this function is not an accurate function , it's effected by os ,
 *		  	  so , if you want to take an accurate action  , like motor controller , this function may take some bad effect .
 */

inline int
BBBIO_sys_delay_ms(unsigned long msec)
{
        struct timespec a;

        a.tv_nsec=(msec) * 1000000L;
        a.tv_sec=0;
        if(nanosleep(&a, NULL) != 0) {
		fprintf(stderr, "delay_ms error: %s\n", strerror(errno));
		return 0;
        }
	return 1;
}

//-------------------------------------------------------------------------------------------
/*********************************
 micro second sleep
 *******************************
 *      @param msec 	: sleep time ,microsecond unit .
 *
 *      @return 	: 1 for success , 0 for failed
 *
 *      @example 	: BBBIO_sys_delay_us(2000); // Sleep 2000us (2ms)
 *
 *      Warring 	: this function is not an accurate function , it's effected by os ,
 *                	  so , if you want to take an accurate action  , like motor controller , this function may take some bad effect .
 */

inline int
BBBIO_sys_delay_us(unsigned long msec)
{
        struct timespec a;

        a.tv_nsec=(msec) * 1000L;
        a.tv_sec=0;
        if (nanosleep(&a, NULL) != 0) {
		fprintf(stderr, "delay_ms error: %s\n", strerror(errno));
        }
        return 0;
}

/*=============================================================================================
 * BBBIO I/O control function (whole GPIO)
 * All function below is unstable
 *
 * pin_high and pin_low is very useful , but in some case , ex : 7-Segment display , it must pin high/low 8 times , it's very waste time .
 *
 * if you need to control many I/O in same time , please try to using BBBIO_GPIO_ prefix funcion ,
 * these function allow you control whole GPIO at once function call .
 *
 */

//-----------------------------------------------------------------------------------------------
/*********************************
 GPIO Set direction
 *******************************
 * Set whole GPIO transmission direction
 *      @param gpio     : GPIO number , BBBIO_GPIO0 / BBBIO_GPIO1 / BBBIO_GPIO2 / BBBIO_GPIO3 .
 *      @param inset    : Inpute pin set , using BBBIO_GPIO_PIN_? and | operator to express it.
 *                                        The pin which not selected will keeping origin status .
 *      @param outset   : Output pin set , format as same as inset .
 *
 *      @return         : 1 for success , 0 for failed
 *
 *      @example        : BBBIO_GPIO_set_dir(BBBIO_GPIO1 ,                              // handle GPIO1
 *                                           BBBIO_GPIO_PIN_11 | BBBIO_GPIO_PIN_12 ,    // pin 11 and 12 as input
 *                                           BBBIO_GPIO_PIN_15 | BBBIO_GPIO_PIN_16);    // pin 15 and 16 as output
 *
 *      Warring         : if you nedd this function , please check value you input , or it may effect other chip or device
 */

int BBBIO_GPIO_set_dir(unsigned int  gpio, unsigned int inset , unsigned int outset)
{
        int param_error=0;
	volatile unsigned int* reg;

        if (memh == 0)			/* sanity checks */
                param_error = 1;

        if (gpio > 3) 	/* GPIO range */
                param_error = 1;

        if (inset == 0 && outset == 0)	/* pin identify error */
                param_error = 1;

        if (param_error) {
#ifdef BBBIO_LIB_DBG
		printf("BBBIO_GPIO_set_dir: parameter error!\n");
#endif
                return -1;
        }

        reg=(void *)gpio_addr[gpio] + BBBIO_GPIO_OE;
        *reg &= ~outset ;
        *reg |= inset ;

        return 0;
}


//-----------------------------------------------------------------------------------------------
/*********************************
 GPIO High / Low
 *******************************
 * Set whole GPIO Output logic
 *      @param gpio     : GPIO number , BBBIO_GPIO0 / BBBIO_GPIO1 / BBBIO_GPIO2 / BBBIO_GPIO3 .
 *      @param pinset    : control pin set , using BBBIO_GPIO_PIN_? and | operator to express it.
 *                         The pin which not selected will keeping origin status .
 *
 *      @return : void
 *
 *      @example : BBBIO_GPIO_high(BBBIO_GPIO1 ,						// control GPIO 1
 *                                 BBBIO_GPIO_PIN_11 | BBBIO_GPIO_PIN_12 | BBBIO_GPIO_PIN_15);	// PIN 11 12 15 pin high simultaneously
 *
 *      Warring : if you nedd this function , please check value you input , or it may effect other chip or device .
 */


// BBBIO_GPIO_SAFE_MASK exporess the usable gpio in BBB Expansion Header P8 and P9 .
// and mask the BBBIO_GPIO_high and BBBIO_GPIO_low to avoid the error access of the pin which not in P8 and P9 .
const unsigned int BBBIO_GPIO_SAFE_MASK [] ={
1<< 2 | 1<<3 | 1<<4 | 1<<5 | 1<<7 | 1<<8 | 1<<9 | 1<<10 | 1<<11 | 1<<12 | 1<<13 | 1<<14 | 1<<15 | 1<<20 | 1<<22 | 1<<23 | 1<<26 | 1<<27 | 1<<30 | 1<<31,        // GPIO 0
1<< 0 | 1<<1 | 1<<2 | 1<<3 | 1<<4 | 1<<5 | 1<<6 | 1<<7 | 1<<12 | 1<<13 | 1<<14 | 1<<15 | 1<<16 | 1<<17 | 1<<18 | 1<<19 | 1<<28 | 1<<29 | 1<<30 | 1<<31, // GPIO 1
1<<1 | 1<<2 | 1<<3 | 1<<4 | 1<<5 | 1<<6 | 1<<7 | 1<<8 | 1<<9 | 1<<10 | 1<<11 | 1<<12 | 1<<13 | 1<<14 | 1<<15 | 1<<16 | 1<<17 | 1<<22 | 1<<23 | 1<<24 | 1<<25 ,  // GPIO 2
1<<14 | 1<<15 | 1<<16 | 1<<17 | 1<<18 | 1<<20 | 1<<19 | 1<<21 };        // GPIO 3

void BBBIO_GPIO_high(unsigned int gpio ,unsigned int pinset)
{
	*((unsigned int *)((void *)gpio_addr[gpio]+BBBIO_GPIO_SETDATAOUT)) = pinset & BBBIO_GPIO_SAFE_MASK[gpio] ;
}

void BBBIO_GPIO_low(unsigned int gpio ,unsigned int pinset)
{
	*((unsigned int *)((void *)gpio_addr[gpio]+BBBIO_GPIO_CLEARDATAOUT)) = pinset &  BBBIO_GPIO_SAFE_MASK[gpio];
}

int BBBIO_GPIO_get(char gpio, unsigned int pinset)
{
	 return *((unsigned int *)((void *)gpio_addr[gpio]+BBBIO_GPIO_DATAIN)) & pinset;
}
