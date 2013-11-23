// BBBiolib.h
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

#ifndef _BBBIOLIB_H_
#define _BBBIOLIB_H_

#define BBBIO_LIB_DBG 1
#define BBBIO_DIR_IN 0
#define BBBIO_DIR_OUT 1

// enable pinmux functionality
// not implemented today
#define BBBIO_PINMUX_EN 0

#define CONTROL_MODULE 0x44e10000
#define CONTROL_LEN 0xA00

// Clock Module Peripheral Registers
/*
*/

#define BBBIO_CM_PER_ADDR			0x44e00000
#define BBBIO_CM_PER_LEN			0x4000
#define BBBIO_CM_PER_L4LS_CLKSTCTRL	0x0
#define BBBIO_CM_PER_GPIO1_CLKCTRL	0xAC
#define BBBIO_CM_PER_GPIO2_CLKCTRL	0xB0
#define BBBIO_CM_PER_GPIO3_CLKCTRL	0xB4


// Clock Module Wakeup Registers
/* @Source : AM335x Technical Reference Manual ,page 976~1040
 *                         Table 8-88. CM_WKUP REGISTERS
 *
*/

#define BBBIO_CM_WKUP_ADDR 			0x44e00400
#define BBBIO_CM_WKUP_OFFSET_FROM_CM_PER	0x400		// for mapping alignment . BBBIO_CM_WKUP_ADDR is not aligned of page boundary in 4k page .
#define BBBIO_CM_WKUP_LEN 			0x100
#define BBBIO_CM_WKUP_GPIO0_CLKCTRL 		0x8		// This register manages the GPIO0 clocks 	, Section 8.1.12.2.3 ,page : 983.


// AM335X GPIO memory mapping address
/* @Source : AM335x Technical Reference Manual ,page 171~173
 			 Table 2-2. L4_WKUP Peripheral Memory Map (continued)
			 Table 2-3. L4_PER Peripheral Memory Map (continued)
*/
#define BBBIO_GPIO0 		0		// GPIO ID
#define BBBIO_GPIO1 		1
#define BBBIO_GPIO2 		2
#define BBBIO_GPIO3		3
#define BBBIO_GPIO0_ADDR	0x44e07000
#define BBBIO_GPIO1_ADDR 	0x4804c000
#define BBBIO_GPIO2_ADDR 	0x481AC000
#define BBBIO_GPIO3_ADDR 	0x481AE000
#define BBBIO_GPIOX_LEN 	0x1000		// GPIO length , ex: GPIO0 0x44e07000 ~ 0x44e07FFF

//#define GPIO1_SIZE (GPIO1_END_ADDR - GPIO1_START_ADDR)
// GPIO control register address
/* @Source : AM335x Technical Reference Manual ,page 4640~4666
 			 Table 25-5. GPIO REGISTERS

			 note : GPIO_SETDATAOUT is single pin setting
					GPIO_DATAOUT is hole pin setting
*/
#define BBBIO_GPIO_OE 			0x134	// Output Data Enable 			, Section 25.4.1.16 ,page : 4656
#define BBBIO_GPIO_DATAIN 		0x138	// Sampled Input Data 			, Section 25.4.1.17 ,page : 4657
#define BBBIO_GPIO_DATAOUT 		0x13C	// Data to set on output pins		, Section 25.4.1.18 ,page : 4658
#define BBBIO_GPIO_CLEARDATAOUT		0x190	// Clear Data Output Register 		, Section 25.4.1.25 ,page : 4665 , 1 is set pin to Low ,  BUT 0 is no effect
#define BBBIO_GPIO_SETDATAOUT		0x194	// Set Data Output Register 		, Section 25.4.1.26 ,page : 4666 , 1 is set pin to HIGH ,  BUT 0 is no effect

#define BBBIO_GPIO_CTRL			0x130	//

// call this first. Returns 0 on success, -1 on failure
int iolib_init(void);
// Set port direction (DIR_IN/DIR_OUT) where port is 8/9 and pin is 1-46
int iolib_setdir(char port, char pin, char dir);
// call this when you are done with I/O. Returns 0 on success, -1 on failure
int iolib_free(void);

// provides an inaccurate delay
// The maximum delay is 999msec
int iolib_delay_ms(unsigned int msec);

// set and get pin levels
inline void pin_high(char port, char pin);
inline void pin_low(char port, char pin);
inline char is_high(char port, char pin);
inline char is_low(char port, char pin);

//---------------------------------------------------------
// call this first. Returns 0 on success, -1 on failure
int BBBIO_sys_init(void);
// call this when you are done with I/O. Returns 0 on success, -1 on failure
int BBBIO_sys_release(void);


void BBBIO_sys_GPIO_status() ;
int BBBIO_sys_Enable_GPIO(unsigned int gpio);		// Enable GPIOx's clock
int BBBIO_sys_Disable_GPIO(unsigned int gpio);		// Disable GPIOx's clock



// Set port direction (BBBIO_DIR_IN /BBBIO_DIR_OUT) where port is 8/9 and pin is 1-46
int BBBIO_set_dir(char port, char pin, char dir);

// set and get pin levels
// (port/pin unit , control single pin at board)
inline void BBBIO_set_high(char port, char pin);
inline void BBBIO_set_low(char port, char pin);
inline char BBBIO_get(char port, char pin);

// (GPIO unit , control hole register)
#define BBBIO_GPIO_PIN_0	1<< 0
#define BBBIO_GPIO_PIN_1	1<< 1
#define BBBIO_GPIO_PIN_2	1<< 2
#define BBBIO_GPIO_PIN_3	1<< 3
#define BBBIO_GPIO_PIN_4	1<< 4
#define BBBIO_GPIO_PIN_5	1<< 5
#define BBBIO_GPIO_PIN_6	1<< 6
#define BBBIO_GPIO_PIN_7	1<< 7
#define BBBIO_GPIO_PIN_8	1<< 8
#define BBBIO_GPIO_PIN_9	1<< 9
#define BBBIO_GPIO_PIN_10	1<< 10
#define BBBIO_GPIO_PIN_11	1<< 11
#define BBBIO_GPIO_PIN_12	1<< 12
#define BBBIO_GPIO_PIN_13	1<< 13
#define BBBIO_GPIO_PIN_14	1<< 14
#define BBBIO_GPIO_PIN_15	1<< 15
#define BBBIO_GPIO_PIN_16	1<< 16
#define BBBIO_GPIO_PIN_17	1<< 17
#define BBBIO_GPIO_PIN_18	1<< 18
#define BBBIO_GPIO_PIN_19	1<< 19
#define BBBIO_GPIO_PIN_20	1<< 20
#define BBBIO_GPIO_PIN_21	1<< 21
#define BBBIO_GPIO_PIN_22	1<< 22
#define BBBIO_GPIO_PIN_23	1<< 23
#define BBBIO_GPIO_PIN_24	1<< 24
#define BBBIO_GPIO_PIN_25	1<< 25
#define BBBIO_GPIO_PIN_26	1<< 26
#define BBBIO_GPIO_PIN_27	1<< 27
#define BBBIO_GPIO_PIN_28	1<< 28
#define BBBIO_GPIO_PIN_29	1<< 29
#define BBBIO_GPIO_PIN_30	1<< 30
#define BBBIO_GPIO_PIN_31	1<< 31

int BBBIO_GPIO_set_dir(unsigned int gpio, unsigned int inset , unsigned int outset);
inline void BBBIO_GPIO_high(unsigned int gpio ,unsigned int pinset);
inline void BBBIO_GPIO_low(unsigned int gpio ,unsigned int pinset);
inline int BBBIO_GPIO_get(char gpio, unsigned int pinset);			// GPIO data register 32 bit

#endif // _IOLIB_H_


