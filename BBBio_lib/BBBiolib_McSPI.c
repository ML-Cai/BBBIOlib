#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sched.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include "BBBiolib.h"
#include "BBBiolib_McSPI.h"
/*----------------------------------------------------------------------------------------------- */
/* Argument define */

/* McSPI module have 4channel , but only pinout2 */
#define MCSPI_ARG_MODULE_COUNT	2
#define MCSPI_ARG_CHANNEL_COUNT	2

/* Device register mamory map */
#define MCSPI0_MMAP_ADDR	0x48030000
#define MCSPI1_MMAP_ADDR	0x481A0000
#define MCSPI_MMAP_LEN	0x1000

/* Device register offset */
#define MCSPI_SYSCONFIG	0x110
#define MCSPI_IRQENABLE	0x11c
#define MCSPI_IRQSTATUS	0x118
#define MCSPI_SYSSTATUS	0x114
#define MCSPI_SYST	0x124
#define MCSPI_MODULCTRL	0x128
#define MCSPI_CH0CONF	0x12C
#define MCSPI_CH0STAT	0x130
#define MCSPI_CH0CTRL	0x134
#define MCSPI_TX0	0x138
#define MCSPI_RX0	0x13C
#define MCSPI_XFERLEVEL	0x17C
#define MCSPI_DAFTX	0x180
#define MCSPI_DAFRX	0x1A0

/* offset between MCSPI_CH0CONF, MCSPI_CH1CONF, MCSPI_CH2CONF, MCSPI_CH3CONF */
#define MCSPI_CH_REG_OFFSET	0x14

#define CM_PER_SPI1_CLKCTRL_IDLEST_MASK	(0x3 <<16)
#define MCSPI_GET_CHSTAT_EOT(a)	((a&0x04)>>2)
#define MCSPI_GET_CHSTAT_TXS(a)	((a&0x02)>>1)
#define MCSPI_GET_CHSTAT_RXS(a)	(a&0x01)

#define MCSPI_GET_SYSSTATUS_RESETDONE(a)	(a&0x01)
/* ----------------------------------------------------------------------------------------------- */
/* struct definition */

#define McSPI_TxRx_Tx		0x01
#define McSPI_TxRx_Rx		0x02

struct BBBIO_McSPI_TxRx_struct
{
	unsigned int Tx ;
	unsigned int Rx ;
	unsigned int flag ;
};

#define McSPI_CH_ENABLE		0x01
#define McSPI_CH_DISABLE	0x02

/* McSPI channel strcut */
struct BBBIO_McSPI_CH_struct
{
	int status ;
	unsigned int Tx ;
	unsigned int Rx ;
	unsigned int flag ;
};

/* McSPI module struct */
struct BBBIO_McSPI_struct
{
	int CM_PER_enable ;
	struct BBBIO_McSPI_CH_struct CH[MCSPI_ARG_CHANNEL_COUNT];
};
/* ----------------------------------------------------------------------------------------------- */
/* Global Variable */
extern int memh;
extern volatile unsigned int *cm_per_addr ;

const unsigned int McSPI_AddressOffset[]={MCSPI0_MMAP_ADDR, MCSPI1_MMAP_ADDR};
volatile unsigned int *mcspi_ptr[MCSPI_ARG_CHANNEL_COUNT] ={NULL};

struct BBBIO_McSPI_struct McSPI_Module[MCSPI_ARG_MODULE_COUNT] ={0} ;
/* ----------------------------------------------------------------------------------------------- */
static inline void write_reg(volatile void *reg_base ,unsigned int offset ,unsigned int data)
{
	*((volatile unsigned int* )(reg_base+offset)) = data;
}
/* ----------------------------------------------------------------------------------------------- */
static inline unsigned int read_reg(volatile void *reg_base ,unsigned int offset)
{
	return *((volatile unsigned int* )(reg_base+offset));
}
/* ----------------------------------------------------------------------------------------------- */
/* McSPI init
 * iolib_init() will run this function automatically
 */
int BBBIO_McSPI_Init()
{
	int i ;
	if (memh == 0) {
		printf("BBBIO_McSPI_Init: memory not mapped?\n");
		return 0;
    	}

	for (i = 0 ; i < MCSPI_ARG_MODULE_COUNT ; i++) {
		mcspi_ptr[i] = mmap(0, MCSPI_MMAP_LEN, PROT_READ | PROT_WRITE, MAP_SHARED, memh, McSPI_AddressOffset[i]);
		if(mcspi_ptr[i] == MAP_FAILED) {
			printf("BBBIO_McSPI_Init: McSPI %d mmap failure!\n",i);
	                return 0;
		}
	}
    return 1;
}
/* ----------------------------------------------------------------------------------------------- */
/* McSPI module work function .
 *
 * BBBIO_McSPI_work provide Tx/Rx data from SPI module .
 *
 *	@param SPI_ID	: SPI module ID .
 *	@param chn	: channel ID , am335x provide channel 0~3 , but only  pin out  channel 0 and 1 .
 *	@param Tx_data 	: Tx data ,ignore this arg in Rx only mode (0 as dummy).
 *	@param Rx_data 	: Rx_data ,ignore this arg in Tx only mode , this arg could be NULL in TxRx mode , means Tx data without Rx data.
 *
 *	@return : 0 for error , 1 for success .
 */
int BBBIO_McSPI_work(unsigned int SPI_ID, unsigned int chn, unsigned int Tx_data, unsigned int *Rx_data)
{
	unsigned int chn_offset =0;
	unsigned int reg_value ;
	unsigned int Tx_flag =0;
	struct BBBIO_McSPI_CH_struct * chn_ptr = NULL;

	/* check CM_PER enable status, or it may caue "Bus error" signal message. */
	if(McSPI_Module[SPI_ID].CM_PER_enable) {
		if(McSPI_Module[SPI_ID].CH[chn].status & McSPI_CH_ENABLE) {
			chn_offset = chn * MCSPI_CH_REG_OFFSET ;
			chn_ptr = &McSPI_Module[SPI_ID].CH[chn];

			/* channel enable */
			write_reg(mcspi_ptr[SPI_ID], MCSPI_CH0CTRL + chn_offset, 1);

			/* set transform data */
			if(chn_ptr->flag & McSPI_TxRx_Tx) {
				chn_ptr->Tx = Tx_data ;
				write_reg(mcspi_ptr[SPI_ID], MCSPI_TX0 +chn_offset, chn_ptr->Tx);
			}
			else {	/* must set a dummy data in Tx reigster of receive only mode .*/
				write_reg(mcspi_ptr[SPI_ID], MCSPI_TX0 +chn_offset, 0);
			}

			/* Tx only waiting EOT
			 * Rx only waiting RXS
			 * TxRx waiting both EOT and RXS 
			 * BBBio not support interrupt yet , using polling for the moment */
			reg_value =0;
			if(chn_ptr->flag & McSPI_TxRx_Tx) {
				while(!MCSPI_GET_CHSTAT_EOT(reg_value)) {
					reg_value = read_reg(mcspi_ptr[SPI_ID], MCSPI_CH0STAT + chn_offset);
					sched_yield();
				}
			}
			/* waiting for RXS */
			if(chn_ptr->flag & McSPI_TxRx_Rx) {
				while(!MCSPI_GET_CHSTAT_RXS(reg_value)) {
					reg_value = read_reg(mcspi_ptr[SPI_ID], MCSPI_CH0STAT + chn_offset);
					sched_yield();
				}
				/* copy receive data */
				if(Rx_data != NULL) {
					chn_ptr->Rx = read_reg(mcspi_ptr[SPI_ID], MCSPI_RX0 + chn_offset);
					*Rx_data = chn_ptr->Rx;
				}
			}

			/* channel disable */
			write_reg(mcspi_ptr[SPI_ID], MCSPI_CH0CTRL + chn_offset, 0);
		}
	}
	else {
#ifdef BBBIO_LIB_DBG
		printf("BBBIO_McSPI_work: McSPI %d CM_PER Clock Gating!\n",SPI_ID);
#endif
		return 0 ;
	}
	return 1 ;
}
/* ----------------------------------------------------------------------------------------------- */
/* McSPI module channel control function .
 * 
 * BBBIO_McSPI_channel_ctrl provide channel configuration .
 *	@param SPI_ID	: SPI module ID (BBBIO_McSPI_SPI0 and BBBIO_McSPI_SPI1).
 *	@param chn	: channel ID , am335x provide channel 0~3 , but only  pin out  channel 0 and 1 .
 *	@param MS 	: master or salve mode , BBBio lib only support Master mode now , slave mode setting is ignored (reserved for future).
 *	@param TRM	: TxRx mode ,support 3 mode :  TxRx mode / Rx Only / Tx Only.
 *	@param CLK_div	: Clock Divider , data range 0x00~0xFF , SPI module default clock is 48M Hz ,divid by 2^CLK_div .
 *	@param CLK_mode	: please see file AM335x TRM , session "24.3.1.3.6 Polarity and Phase" , page 4537
 *	@param EPOL	: SPI EN polarity , SPIEN is held high/low during the active state (BBBIO_McSPI_EN_ACT_HIGH/LOW). 
 *	@param DataDir	: Data pin status ,data0 Output data1 Input (BBBIO_McSPI_D0O_D1I) ,or data0 Input data1 Output (BBBIO_McSPI_D0I_D1O)
 *	@param WL	: word length per work , 4-bits ~ 32-bits, 
 *
 *	@return : 0 for error , 1 for success .
*/
int BBBIO_McSPI_channel_ctrl(unsigned int SPI_ID ,		/* SPI module ID , am335x have SPI1 and SPI0*/
				unsigned int channel ,		/* channel ID , channel 0~3 */
				unsigned int MS , 		/* MS 			, maset or slave */
				unsigned int TRM ,		/* TRM 			, Tx only / Rx only , TxRx */
				unsigned int CLK_div ,		/* Clock Divider	, default clock : 48M Hz */
				unsigned int CLKmode ,		/* POL/PHA 		, Clock polarity */
				unsigned int EPOL , 		/* EPOL 		, SPI EN polarity */
				unsigned int DataDir ,		/* IS/DPE1/DPE0		, data0 Output data1 Input ,or data0 Input data1 Output */
				unsigned int WL)		/* WL 			, word length */
{
	int param_error=0;
	unsigned int chn_offset =channel * 0x14;

	if(!McSPI_Module[SPI_ID].CM_PER_enable) {
		printf("BBBIO_McSPI_channel_ctrl: McSPI %d CM_PER Clock Gated!\n",SPI_ID);
		param_error =1;
	}
	else if(MS == BBBIO_McSPI_Slave) {
		printf("BBBIO_McSPI_channel_ctrl: Sorry , BBBio only for master mode .\n");
		param_error =1;
	}
	else if(WL <4 || WL >32)
		param_error =1;

	/* check CM_PER enable status, or it may caue "Bus error" signal message. */
	if(!param_error) {
		/* disable channel */
		write_reg(mcspi_ptr[SPI_ID], MCSPI_CH0CTRL + chn_offset, 0);

		/* channel setting  */
		write_reg(mcspi_ptr[SPI_ID], MCSPI_MODULCTRL, MS << 2);
 		write_reg(mcspi_ptr[SPI_ID], MCSPI_CH0CONF + chn_offset, (DataDir << 16 | TRM << 12 | (WL - 1) << 7 | EPOL << 6 | CLK_div << 2 | CLKmode));

		if(TRM != BBBIO_McSPI_Tx_Only) {
			McSPI_Module[SPI_ID].CH[channel].flag |= McSPI_TxRx_Rx ;
			McSPI_Module[SPI_ID].CH[channel].Rx =0;
		}
	        if(TRM != BBBIO_McSPI_Rx_Only) {
        	        McSPI_Module[SPI_ID].CH[channel].flag |= McSPI_TxRx_Tx ;
			McSPI_Module[SPI_ID].CH[channel].Tx =0;
		}
		McSPI_Module[SPI_ID].CH[channel].status |=McSPI_CH_ENABLE;
	}
	else {
		return 0;
	}
	return 1 ;
}
/* ----------------------------------------------------------------------------------------------- */
/* Beaglebone Black , SPI Expansion Header Mode Check */
int BBBIO_McSPI_EP_check(unsigned int SPI_ID)
{
	unsigned int ret = 1;

	if(SPI_ID ==1) {
		/*SPI 1 cs 0~1*/
		if(!BBBIO_sys_pinmux_check(9, 19, BBBIO_PINMUX_MODE_4) &&
			!BBBIO_sys_pinmux_check(9, 20, BBBIO_PINMUX_MODE_4) &&
			!BBBIO_sys_pinmux_check(9, 28, BBBIO_PINMUX_MODE_3) &&
			!BBBIO_sys_pinmux_check(9, 42, BBBIO_PINMUX_MODE_2) ) {
			ret = 0;
		}

		/* SPI 1 Da0~1 , clk */
		if(!BBBIO_sys_pinmux_check(9, 29, BBBIO_PINMUX_MODE_3) &&
			!BBBIO_sys_pinmux_check(9, 30, BBBIO_PINMUX_MODE_3) &&
			!BBBIO_sys_pinmux_check(9, 31, BBBIO_PINMUX_MODE_3) &&
			!BBBIO_sys_pinmux_check(9, 42, BBBIO_PINMUX_MODE_4)) {
			ret =0;
		}
	}
	else {
		/* SPI 0 cs 0 */
		if(!BBBIO_sys_pinmux_check(9, 17, BBBIO_PINMUX_MODE_0)) {
                        ret = 0;
                }

                /* SPI 1 Da0~1 , clk */
                if(!BBBIO_sys_pinmux_check(9, 18, BBBIO_PINMUX_MODE_0) &&
                        !BBBIO_sys_pinmux_check(9, 21, BBBIO_PINMUX_MODE_0) &&
                        !BBBIO_sys_pinmux_check(9, 22, BBBIO_PINMUX_MODE_0)) {
                        ret =0;
                }
	}
	return ret ;
}
/* ----------------------------------------------------------------------------------------------- */
/* McSPI module Enable/Disable function
 *
 * In fact , clock control function . Disable means gating clock of spi module . 
 *	@param SPI_ID	: SPI module ID .
 *	@param enable	: enable/disable clock .
 *	@param idle	: reserved for future.
 *
 *	@note :
 *		#define BBBIO_McSPI_Enable(a) BBBIO_McSPI_CLK_set(a,1,0)
 *		#define BBBIO_McSPI_Disable(a) BBBIO_McSPI_CLK_set(a,0,0)
 */

int BBBIO_McSPI_CLK_set(unsigned int SPI_ID ,int enable , int idle)
{
	const unsigned int CM_PER_McSPI[] = {BBBIO_CM_PER_SPI0_CLKCTRL, BBBIO_CM_PER_SPI1_CLKCTRL};

	if(SPI_ID > 1) {
		printf("BBBIO_McSPI_CLK_set: Unknow McSPI module ID : %d\n", SPI_ID);
		return 0 ;
	}

	/* pin mux check */
	if(!BBBIO_McSPI_EP_check(SPI_ID)) {
		printf("BBBIO_McSPI_CLK_set: No effect pin mux Detected of SPI-%d ,Please use EP_STATUS toolkit to check Pin mux mode\n", SPI_ID);
		return 0;
	}

	if(enable) {
		write_reg(cm_per_addr, CM_PER_McSPI[SPI_ID], 0x3 << 16 | 0x2);
		McSPI_Module[SPI_ID].CM_PER_enable = 1;
	}
	else {
		write_reg(cm_per_addr, CM_PER_McSPI[SPI_ID], 0);
		McSPI_Module[SPI_ID].CM_PER_enable = 0;
	}

	return 1 ;
}
/* ----------------------------------------------------------------------------------------------- */
void BBBIO_McSPI_Reset(unsigned int SPI_ID)
{
	unsigned int reg_value ;

	/* reset */
	write_reg(mcspi_ptr[SPI_ID], MCSPI_SYSCONFIG, 0x2 << 3 | 0x2);

	/* waiting reset finish */
	reg_value =0;
	while(!MCSPI_GET_SYSSTATUS_RESETDONE(reg_value)) {
		reg_value = read_reg(mcspi_ptr[SPI_ID] ,MCSPI_SYSSTATUS);
		sched_yield();
	}
#ifdef BBBIO_LIB_DBG
	printf("Reset Finish\n");
#endif
}
