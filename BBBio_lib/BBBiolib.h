/* BBBiolib.h
 * Simple I/O library of Beaglebone balck
 *
 * this fnnction is under construction , so mix some libio and BBBIO express ,
 *     libio : support some basic function for Beaglebone black I/O .
 *     BBBIO : support I/O function using GPIO as unit .
 *
 * all function will integrated as soon as fast .
 *
 * v1   October 2013 - shabaz (iolib)
 * v2   October 2013 - shabaz (iolib)
 * v2.1 November 2013 - VagetableAvenger (BBBlib)
 * v2.2 November 2013 - VagetableAvenger (BBBlib) : add GPIO Enable/Disable function
 * v2.4 November.23 2013 - VagetableAvenger (BBBlib) : add whole GPIO control function (I/O and direction set)
*/

#ifndef _BBBIOLIB_H_
#define _BBBIOLIB_H_

#ifdef  __cplusplus
extern "C" {
#endif

/* ----------------------------------------------------------------------- */
#include "BBBiolib_PWMSS.h"
#include "BBBiolib_McSPI.h"
#include "BBBiolib_ADCTSC.h"


#define BBBIO_GPIO_MODULE	0x1
#define BBBIO_PWM_MODULE	0x2
#define BBBIO_MCSPI_MODULE	0x4
#define BBBIO_ADCTSC_MODULE	0x8

//#define BBBIO_GPIO0_MODULE	(BBBIO_GPIO_MODULE << 8 | BBBIO_GPIO_0)





#define BBBIO_LIB_DBG
#define BBBIO_DIR_IN 0
#define BBBIO_DIR_OUT 1


/* ----------------------------------------------------------------------- */
/* enable pinmux functionality
 * not implemented yet (User space not support , privilige invalid)
 */
#define BBBIO_PINMUX_EN 0


#define BBBIO_PINMUX_SLEWCTRL	(1<<6)
#define BBBIO_PINMUX_FAST_RATE	(BBBIO_PINMUX_SLEWCTRL | (0<<6)<<8)
#define BBBIO_PINMUX_SLOW_RATE	(BBBIO_PINMUX_SLEWCTRL | (1<<6)<<8)

#define BBBIO_PINMUX_RXACTIVE	(1<<5)
#define BBBIO_PINMUX_RX_DISABLE	(BBBIO_PINMUX_RXACTIVE | (0<<5)<<8)
#define BBBIO_PINMUX_RX_ENABLE	(BBBIO_PINMUX_RXACTIVE | (1<<5)<<8)

#define BBBIO_PINMUX_PUTYPESEL	(1<<4)
#define BBBIO_PINMUX_PULLDOWN	(BBBIO_PINMUX_PUTYPESEL | (0<<4)<<8)
#define BBBIO_PINMUX_PULLUP	(BBBIO_PINMUX_PUTYPESEL | (1<<4)<<8)

#define BBBIO_PINMUX_PUDEN	(1<<3)
#define BBBIO_PINMUX_PULL_ENABLE	(BBBIO_PINMUX_PUDEN | (0<<3)<<8)
#define BBBIO_PINMUX_PULL_DISABLE	(BBBIO_PINMUX_PUDEN | (1<<3)<<8)

#define BBBIO_PINMUX_MODE	(0x7)
#define BBBIO_PINMUX_MODE_0	(BBBIO_PINMUX_MODE | (0x0)<<8)
#define BBBIO_PINMUX_MODE_1	(BBBIO_PINMUX_MODE | (0x1)<<8)
#define BBBIO_PINMUX_MODE_2	(BBBIO_PINMUX_MODE | (0x2)<<8)
#define BBBIO_PINMUX_MODE_3	(BBBIO_PINMUX_MODE | (0x3)<<8)
#define BBBIO_PINMUX_MODE_4	(BBBIO_PINMUX_MODE | (0x4)<<8)
#define BBBIO_PINMUX_MODE_5	(BBBIO_PINMUX_MODE | (0x5)<<8)
#define BBBIO_PINMUX_MODE_6	(BBBIO_PINMUX_MODE | (0x6)<<8)
#define BBBIO_PINMUX_MODE_7	(BBBIO_PINMUX_MODE | (0x7)<<8)

int BBBIO_sys_pinmux_check(unsigned int port, unsigned int pin, unsigned int Cflag);

/* ----------------------------------------------------------------------
 * Control Module Registers
 * 	@Source : AM335x Technical Reference Manual ,page 1123~1127 , Table 9-10. CONTROL_MODULE REGISTERS
 *
 * 	@note : this library only include some register .
*/
#define BBBIO_CONTROL_MODULE 0x44e10000
#define BBBIO_CONTROL_LEN 0x2000

/* dummy offset (not really register offset) */
#define BBBIO_EXPANSION_HEADER_GND		0xF0000000
#define BBBIO_EXPANSION_HEADER_DC_33V		0xF0000001
#define BBBIO_EXPANSION_HEADER_VDD_5V		0xF0000002
#define BBBIO_EXPANSION_HEADER_SYS_5V		0xF0000003
#define BBBIO_EXPANSION_HEADER_PWR_BUT		0xF0000004
#define BBBIO_EXPANSION_HEADER_SYS_RESETN	0xF0000005
#define BBBIO_EXPANSION_HEADER_VADC		0xF0000006
#define BBBIO_EXPANSION_HEADER_AIN4		0xF0000007
#define BBBIO_EXPANSION_HEADER_AGND		0xF0000008
#define BBBIO_EXPANSION_HEADER_AIN6		0xF0000009
#define BBBIO_EXPANSION_HEADER_AIN5             0xF000000A
#define BBBIO_EXPANSION_HEADER_AIN2             0xF000000B
#define BBBIO_EXPANSION_HEADER_AIN3             0xF000000C
#define BBBIO_EXPANSION_HEADER_AIN0             0xF000000D
#define BBBIO_EXPANSION_HEADER_AIN1             0xF000000E
#define BBBIO_EXPANSION_HEADER_UNKNOW           0xF000000F

/* register offset */
#define BBBIO_CONTROL_STATUS	0x40
#define BBBIO_PWMSS_CTRL	0x664
#define BBBIO_CONF_GPMC_AD0	0x800
#define BBBIO_CONF_GPMC_AD1	0x804
#define BBBIO_CONF_GPMC_AD2	0x808
#define BBBIO_CONF_GPMC_AD3	0x80C
#define BBBIO_CONF_GPMC_AD4	0x810
#define BBBIO_CONF_GPMC_AD5	0x814
#define BBBIO_CONF_GPMC_AD6	0x818
#define BBBIO_CONF_GPMC_AD7	0x81C
#define BBBIO_CONF_GPMC_AD8	0x820
#define BBBIO_CONF_GPMC_AD9	0x824
#define BBBIO_CONF_GPMC_AD10	0x828
#define BBBIO_CONF_GPMC_AD11	0x82C
#define BBBIO_CONF_GPMC_AD12	0x830
#define BBBIO_CONF_GPMC_AD13	0x834
#define BBBIO_CONF_GPMC_AD14	0x838
#define BBBIO_CONF_GPMC_AD15	0x83C
#define BBBIO_CONF_GPMC_A0	0x840
#define BBBIO_CONF_GPMC_A1	0x844
#define BBBIO_CONF_GPMC_A2	0x848
#define BBBIO_CONF_GPMC_A3	0x84C
#define BBBIO_CONF_GPMC_A4	0x850
#define BBBIO_CONF_GPMC_A5	0x854
#define BBBIO_CONF_GPMC_A6	0x858
#define BBBIO_CONF_GPMC_A7	0x85C
#define BBBIO_CONF_GPMC_A8	0x860
#define BBBIO_CONF_GPMC_A9	0x864
#define BBBIO_CONF_GPMC_A10	0x868
#define BBBIO_CONF_GPMC_A11	0x86C
#define BBBIO_CONF_GPMC_WAIT0	0x870
#define BBBIO_CONF_GPMC_WPN	0x874
#define BBBIO_CONF_GPMC_BEN1	0x878
#define BBBIO_CONF_GPMC_CSN0	0x87C
#define BBBIO_CONF_GPMC_CSN1	0x880
#define BBBIO_CONF_GPMC_CSN2	0x884
#define BBBIO_CONF_GPMC_CSN3	0x888
#define BBBIO_CONF_GPMC_CLK	0x88C
#define BBBIO_CONF_GPMC_ADVN_ALE	0x890
#define BBBIO_CONF_GPMC_OEN_REN	0x894
#define BBBIO_CONF_GPMC_WEN	0x898
#define BBBIO_CONF_GPMC_BEN0_CLE	0x89C
#define BBBIO_CONF_LCD_DATA0	0x8A0
#define BBBIO_CONF_LCD_DATA1	0x8A4
#define BBBIO_CONF_LCD_DATA2	0x8A8
#define BBBIO_CONF_LCD_DATA3	0x8AC
#define BBBIO_CONF_LCD_DATA4	0x8B0
#define BBBIO_CONF_LCD_DATA5	0x8B4
#define BBBIO_CONF_LCD_DATA6	0x8B8
#define BBBIO_CONF_LCD_DATA7	0x8BC
#define BBBIO_CONF_LCD_DATA8	0x8C0
#define BBBIO_CONF_LCD_DATA9	0x8C4
#define BBBIO_CONF_LCD_DATA10	0x8C8
#define BBBIO_CONF_LCD_DATA11	0x8CC
#define BBBIO_CONF_LCD_DATA12	0x8D0
#define BBBIO_CONF_LCD_DATA13	0x8D4
#define BBBIO_CONF_LCD_DATA14	0x8D8
#define BBBIO_CONF_LCD_DATA15	0x8DC
#define BBBIO_CONF_LCD_VSYNC	0x8E0
#define BBBIO_CONF_LCD_HSYNC	0x8E4
#define BBBIO_CONF_LCD_PCLK	0x8E8
#define BBBIO_CONF_LCD_AC_BIAS_EN	0x8EC
#define BBBIO_CONF_MMC0_DAT3	0x8F0
#define BBBIO_CONF_MMC0_DAT2	0x8F4
#define BBBIO_CONF_MMC0_DAT1	0x8F8
#define BBBIO_CONF_MMC0_DAT0	0x8FC
#define BBBIO_CONF_MMC0_CLK	0x900
#define BBBIO_CONF_MMC0_CMD	0x904
#define BBBIO_CONF_MII1_COL	0x908
#define BBBIO_CONF_MII1_CRS	0x90C
#define BBBIO_CONF_MII1_RX_ER	0x910
#define BBBIO_CONF_MII1_TX_EN	0x914
#define BBBIO_CONF_MII1_RX_DV	0x918
#define BBBIO_CONF_MII1_TXD3	0x91C
#define BBBIO_CONF_MII1_TXD2	0x920
#define BBBIO_CONF_MII1_TXD1	0x924
#define BBBIO_CONF_MII1_TXD0	0x928
#define BBBIO_CONF_MII1_TX_CLK	0x92C
#define BBBIO_CONF_MII1_RX_CLK	0x930
#define BBBIO_CONF_MII1_RXD3	0x934
#define BBBIO_CONF_MII1_RXD2	0x938
#define BBBIO_CONF_MII1_RXD1	0x93C
#define BBBIO_CONF_MII1_RXD0	0x940
#define BBBIO_CONF_RMII1_REF_CLK	0x944
#define BBBIO_CONF_MDIO		0x948
#define BBBIO_CONF_MDC		0x94C
#define BBBIO_CONF_SPI0_SCLK	0x950
#define BBBIO_CONF_SPI0_D0	0x954
#define BBBIO_CONF_SPI0_D1	0x958
#define BBBIO_CONF_SPI0_CS0	0x95C
#define BBBIO_CONF_SPI0_CS1	0x960
#define BBBIO_CONF_ECAP0_IN_PWM0_OUT	0x964
#define BBBIO_CONF_UART0_CTSN	0x968
#define BBBIO_CONF_UART0_RTSN	0x96C
#define BBBIO_CONF_UART0_RXD	0x970
#define BBBIO_CONF_UART0_TXD	0x974
#define BBBIO_CONF_UART1_CTSN	0x978
#define BBBIO_CONF_UART1_RTSN	0x97C
#define BBBIO_CONF_UART1_RXD	0x980
#define BBBIO_CONF_UART1_TXD	0x984
#define BBBIO_CONF_I2C0_SDA	0x988
#define BBBIO_CONF_I2C0_SCL	0x98C
#define BBBIO_CONF_MCASP0_ACLKX	0x990
#define BBBIO_CONF_MCASP0_FSX	0x994
#define BBBIO_CONF_MCASP0_AXR0	0x998
#define BBBIO_CONF_MCASP0_AHCLKR	0x99C
#define BBBIO_CONF_MCASP0_ACLKR	0x9A0
#define BBBIO_CONF_MCASP0_FSR	0x9A4
#define BBBIO_CONF_MCASP0_AXR1	0x9A8
#define BBBIO_CONF_MCASP0_AHCLKX	0x9AC
#define BBBIO_CONF_XDMA_EVENT_INTR0	0x9B0
#define BBBIO_CONF_XDMA_EVENT_INTR1	0x9B4
#define BBBIO_CONF_WARMRSTN	0x9B8
#define BBBIO_CONF_NNMI		0x9C0
#define BBBIO_CONF_TMS		0x9D0
#define BBBIO_CONF_TDI		0x9D4
#define BBBIO_CONF_TDO		0x9D8
#define BBBIO_CONF_TCK		0x9DC

/* ---------------------------------------------------------------------- */
/* Clock Module Peripheral Registers
 *	@Source : AM335x Technical Reference Manual ,page 916 , Table 8-29. CM_PER REGISTERS
 *
 */

#define BBBIO_CM_PER_ADDR		0x44e00000
#define BBBIO_CM_PER_LEN		0x4000

/* register offset */
#define BBBIO_CM_PER_L4LS_CLKSTCTRL	0x0
#define BBBIO_CM_PER_GPIO1_CLKCTRL	0xAC
#define BBBIO_CM_PER_GPIO2_CLKCTRL	0xB0
#define BBBIO_CM_PER_GPIO3_CLKCTRL	0xB4
#define BBBIO_CM_PER_EPWMSS0_CLKCTRL	0xD4
#define BBBIO_CM_PER_EPWMSS1_CLKCTRL	0xCC
#define BBBIO_CM_PER_EPWMSS2_CLKCTRL	0xD8
#define BBBIO_CM_PER_SPI0_CLKCTRL	0x4C
#define BBBIO_CM_PER_SPI1_CLKCTRL       0x50


/* ----------------------------------------------------------------------
 * Clock Module Wakeup Registers
 *	@Source : AM335x Technical Reference Manual ,page 976 , Table 8-88. CM_WKUP REGISTERS
 *
*/

#define BBBIO_CM_WKUP_ADDR 			0x44e00400
#define BBBIO_CM_WKUP_OFFSET_FROM_CM_PER	0x400		/* for mapping alignment . BBBIO_CM_WKUP_ADDR is not aligned of page boundary in 4k page .*/
#define BBBIO_CM_WKUP_LEN 			0x100

/* register offset */
#define BBBIO_CM_WKUP_GPIO0_CLKCTRL 		0x8
#define BBBIO_CM_WKUP_ADC_TSC_CLKCTRL		0xBC

/* ----------------------------------------------------------------------
 * GPIO Module Registers
 *	@Source : AM335x Technical Reference Manual ,page 4640~4666 , Table 25-5. GPIO REGISTERS
 *
 *	@note : GPIO_SETDATAOUT is single pin setting , GPIO_DATAOUT is hole pin setting
 */
#define BBBIO_GPIO0 		0		/* GPIO ID */
#define BBBIO_GPIO1 		1
#define BBBIO_GPIO2 		2
#define BBBIO_GPIO3		3
#define BBBIO_GPIO0_ADDR	0x44e07000
#define BBBIO_GPIO1_ADDR 	0x4804c000
#define BBBIO_GPIO2_ADDR 	0x481AC000
#define BBBIO_GPIO3_ADDR 	0x481AE000
#define BBBIO_GPIOX_LEN 	0x1000		/* GPIO length , ex: GPIO0 0x44e07000 ~ 0x44e07FFF */

/* register offset */
#define BBBIO_GPIO_OE			0x134
#define BBBIO_GPIO_DATAIN 		0x138
#define BBBIO_GPIO_DATAOUT 		0x13C
#define BBBIO_GPIO_CLEARDATAOUT		0x190
#define BBBIO_GPIO_SETDATAOUT		0x194

#define BBBIO_GPIO_CTRL			0x130
#define BBBIO_GPIO_DEBOUNCENABLE	0x150
#define BBBIO_GPIO_DEBOUNCINGTIME	0x154


/* ----------------------------------------------------------------------
 * iolib Basic function
 *
 */

/* call this first. Returns 0 on success, -1 on failure*/
int iolib_init(void);

/* Set port direction (DIR_IN/DIR_OUT) where port is 8/9 and pin is 1-46 */
int iolib_setdir(char port, char pin, char dir);

/* call this when you are done with I/O. Returns 0 on success, -1 on failure */
int iolib_free(void);


/* provides an inaccurate delay ,The maximum delay is 999 msec */
int iolib_delay_ms(unsigned int msec);

/* set and get pin levels */
void pin_high(char port, char pin);
void pin_low(char port, char pin);
char is_high(char port, char pin);
char is_low(char port, char pin);

/* ----------------------------------------------------------------------
 * BBBIO basic function
 */

/* call this first. Returns 0 on success, -1 on failure */
int BBBIO_sys_init(int flag);

/* call this when you are done with I/O. Returns 0 on success, -1 on failure */
int BBBIO_sys_release(void);


void BBBIO_sys_GPIO_status() ;
void BBBIO_sys_Expansion_Header_status(unsigned int port) ;

/* ----------------------------------------------------------------------
 * BBBIO GPIO function
 */

/*  Enable /Disable GPIOx's Clock */
int BBBIO_sys_Enable_GPIO(unsigned int gpio);
int BBBIO_sys_Disable_GPIO(unsigned int gpio);

/* port/pin unit , control "Single" pin one time
 *
 * port is 8/9 and pin is 1-46 .
 *
 *	BBBIO_set_dir : Set port direction .
 *	BBBIO_set_high /BBBIO_set_low : set  pin logic levels.
 *	BBBIO_get : get pin logic levels.
 */
int BBBIO_set_dir(char port, char pin, char dir);
inline void BBBIO_set_high(char port, char pin);
inline void BBBIO_set_low(char port, char pin);
inline char BBBIO_get(char port, char pin);


/* GPIO unit , control "Whole" GPIO pin one time
 *
 * inset / outset / pinset express the pin which need control .
 *
 *	BBBIO_GPIO_set_dir : Set port direction .
 *	BBBIO_GPIO_high /BBBIO_GPIO_low : set  pin logic levels.
 *	BBBIO_GPIO_get : get pin logic levels.
 */
#define BBBIO_GPIO_PIN_0	(1<< 0)
#define BBBIO_GPIO_PIN_1	(1<< 1)
#define BBBIO_GPIO_PIN_2	(1<< 2)
#define BBBIO_GPIO_PIN_3	(1<< 3)
#define BBBIO_GPIO_PIN_4	(1<< 4)
#define BBBIO_GPIO_PIN_5	(1<< 5)
#define BBBIO_GPIO_PIN_6	(1<< 6)
#define BBBIO_GPIO_PIN_7	(1<< 7)
#define BBBIO_GPIO_PIN_8	(1<< 8)
#define BBBIO_GPIO_PIN_9	(1<< 9)
#define BBBIO_GPIO_PIN_10	(1<< 10)
#define BBBIO_GPIO_PIN_11	(1<< 11)
#define BBBIO_GPIO_PIN_12	(1<< 12)
#define BBBIO_GPIO_PIN_13	(1<< 13)
#define BBBIO_GPIO_PIN_14	(1<< 14)
#define BBBIO_GPIO_PIN_15	(1<< 15)
#define BBBIO_GPIO_PIN_16	(1<< 16)
#define BBBIO_GPIO_PIN_17	(1<< 17)
#define BBBIO_GPIO_PIN_18	(1<< 18)
#define BBBIO_GPIO_PIN_19	(1<< 19)
#define BBBIO_GPIO_PIN_20	(1<< 20)
#define BBBIO_GPIO_PIN_21	(1<< 21)
#define BBBIO_GPIO_PIN_22	(1<< 22)
#define BBBIO_GPIO_PIN_23	(1<< 23)
#define BBBIO_GPIO_PIN_24	(1<< 24)
#define BBBIO_GPIO_PIN_25	(1<< 25)
#define BBBIO_GPIO_PIN_26	(1<< 26)
#define BBBIO_GPIO_PIN_27	(1<< 27)
#define BBBIO_GPIO_PIN_28	(1<< 28)
#define BBBIO_GPIO_PIN_29	(1<< 29)
#define BBBIO_GPIO_PIN_30	(1<< 30)
#define BBBIO_GPIO_PIN_31	(1<< 31)

int BBBIO_GPIO_set_dir(unsigned int gpio, unsigned int inset , unsigned int outset);
void BBBIO_GPIO_high(unsigned int gpio ,unsigned int pinset);
void BBBIO_GPIO_low(unsigned int gpio ,unsigned int pinset);
int BBBIO_GPIO_get(char gpio, unsigned int pinset);

#ifdef __cplusplus
 }
#endif

#endif // _IOLIB_H_


