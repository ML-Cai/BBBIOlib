// BBBiolib.h
// Simple I/O library of Beaglebone balck
// v1 	October 2013 - shabaz (iolib)
// v2 	October 2013 - shabaz (iolib)
// V2.1 November 2013 - VagetableAvenger (BBBlib) : add some comment and modify function name

#ifndef _BBBIOLIB_H_
#define _BBBIOLIB_H_

#define BBBIO_LIB_DBG 0
#define BBBIO_DIR_IN 0
#define BBBIO_DIR_OUT 1

// enable pinmux functionality
// not implemented today
#define BBBIO_PINMUX_EN 0

#define BBBIO_CONTROL_MODULE 0x44e10000
#define BBBIO_CONTROL_LEN 0xA00

// AM335X GPIO memory mapping address
/* @Source : AM335x Technical Reference Manual ,page 171~173
 			 Table 2-2. L4_WKUP Peripheral Memory Map (continued)
			 Table 2-3. L4_PER Peripheral Memory Map (continued)
*/
#define BBBIO_GPIO0 0x44e07000
#define BBBIO_GPIO1 0x4804c000
#define BBBIO_GPIO2 0x481ac000
#define BBBIO_GPIO3 0x481ae000
#define BBBIO_GPIOX_LEN 0x1000	// GPIO length , ex: GPIO0 0x44e07000 ~ 0x44e07FFF

//#define GPIO1_SIZE (GPIO1_END_ADDR - GPIO1_START_ADDR)
// GPIO control register address
/* @Source : AM335x Technical Reference Manual ,page 4640~4666
 			 Table 25-5. GPIO REGISTERS
			 
			 note : GPIO_SETDATAOUT is single pin setting
					GPIO_DATAOUT is hole pin setting
*/
#define BBBIO_GPIO_OE 			0x134	// Output Data Enable 			, Section 25.4.1.16 ,page : 4656
#define BBBIO_GPIO_DATAIN 		0x138	// Sampled Input Data 			, Section 25.4.1.17 ,page : 4657
#define BBBIO_GPIO_DATAOUT 		0x13C	// Data to set on output pins	, Section 25.4.1.18 ,page : 4658
#define BBBIO_GPIO_CLEARDATAOUT 0x190	// Clear Data Output Register 	, Section 25.4.1.25 ,page : 4665 , 1 is set pin to Low ,  BUT 0 is no effect
#define BBBIO_GPIO_SETDATAOUT 	0x194	// Set Data Output Register 	, Section 25.4.1.26 ,page : 4666 , 1 is set pin to HIGH ,  BUT 0 is no effect

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
int BBBIO_init(void);
// call this when you are done with I/O. Returns 0 on success, -1 on failure
int BBBIO_release(void);

// Set port direction (DIR_IN/DIR_OUT) where port is 8/9 and pin is 1-46
int BBBIO_set_dir(char port, char pin, char dir);

// set and get pin levels
// (pin unit)
inline void BBBIO_set_high(char port, char pin);
inline void BBBIO_set_low(char port, char pin);
inline char BBBIO_get(char port, char pin);

// (port unit)
inline void BBBIO_set_high_port(char port, char pin);
inline void BBBIO_set_low_port(char port, char pin);
inline int BBBIO_get_port(char port, char pin);			// GPIO data register 32 bit

#endif // _IOLIB_H_


