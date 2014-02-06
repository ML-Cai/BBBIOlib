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
//-----------------------------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------------------------
/* struct definition */

#define McSPI_TxRx_Tx		0x01
#define McSPI_TxRx_Rx		0x02

struct BBBIO_McSPI_TxRx_struct
{
	unsigned int Tx ;
	unsigned int Rx ;
	unsigned int flag ;
};

// Channel struct
#define McSPI_CH_ENABLE		0x01
#define McSPI_CH_DISABLE	0x02

/* McSPI channel strcut */
struct BBBIO_McSPI_CH_struct
{
	int status ;
	struct BBBIO_McSPI_TxRx_struct TxRx ;
};

/* McSPI module struct */
struct BBBIO_McSPI_struct
{
	int CM_PER_enable ;
	struct BBBIO_McSPI_CH_struct CH[MCSPI_ARG_CHANNEL_COUNT];
};
//-----------------------------------------------------------------------------------------------
/* Global Variable */
extern int memh;
extern volatile unsigned int *cm_per_addr ;

const unsigned int McSPI_AddressOffset[]={MCSPI0_MMAP_ADDR, MCSPI1_MMAP_ADDR};
volatile unsigned int *mcspi_ptr[MCSPI_ARG_CHANNEL_COUNT] ={NULL};

struct BBBIO_McSPI_struct McSPI_Module[MCSPI_ARG_MODULE_COUNT] ={0} ;
/* ----------------------------------------------------------------------------------------------- */
inline void write_reg(volatile void *reg_base ,unsigned int offset ,unsigned int data)
{
	*((volatile unsigned int* )(reg_base+offset)) = data;
}
/* ----------------------------------------------------------------------------------------------- */
inline unsigned int read_reg(volatile void *reg_base ,unsigned int offset)
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
#ifdef BBBIO_LIB_DBG
		printf("BBBIO_PWM_Init: memory not mapped?\n");
#endif
		return 0;
    	}

	for (i=0; i<2; i++) {
		mcspi_ptr[i] = mmap(0, MCSPI_MMAP_LEN, PROT_READ | PROT_WRITE, MAP_SHARED, memh, McSPI_AddressOffset[i]);
		if(mcspi_ptr[i] == MAP_FAILED) {
#ifdef BBBIO_LIB_DBG
			printf("BBBIO_McSPI_Init: McSPI %d mmap failure!\n",i);
#endif
	                return 0;
		}
	}
    return 1;
}
/* ----------------------------------------------------------------------------------------------- */
/* Set Transmit data */
void BBBIO_McSPI_Tx(unsigned int McSPI_ID ,unsigned int channel ,unsigned int data)
{
	McSPI_Module[McSPI_ID].CH[channel].TxRx.Tx =data;
}
/* ----------------------------------------------------------------------------------------------- */
/* Receive data */
unsigned int BBBIO_McSPI_Rx(unsigned int McSPI_ID ,unsigned int channel )
{
	return McSPI_Module[McSPI_ID].CH[channel].TxRx.Rx ;
}
/* ----------------------------------------------------------------------------------------------- */
/* McSPI module work */
int BBBIO_McSPI_work(unsigned int McSPI_ID)
{
	unsigned int chn =0;
	unsigned int chn_offset =0;
	unsigned int reg_value ;

	/* check CM_PER enable status, or it may caue "Bus error" signal message. */
	if(McSPI_Module[McSPI_ID].CM_PER_enable) {
		struct BBBIO_McSPI_CH_struct *CH_ptr = &McSPI_Module[McSPI_ID].CH[0] ;

		for(chn =0 ; chn < MCSPI_ARG_CHANNEL_COUNT ; chn ++) {
			if(McSPI_Module[McSPI_ID].CH[chn].status &McSPI_CH_ENABLE) {
				chn_offset = chn * MCSPI_CH_REG_OFFSET ;

				/* channel enable */
				write_reg(mcspi_ptr[McSPI_ID], MCSPI_CH0CTRL + chn_offset, 1);

				/* set transform data */
				if(McSPI_Module[McSPI_ID].CH[chn].TxRx.flag & McSPI_TxRx_Tx) {
					write_reg(mcspi_ptr[McSPI_ID], MCSPI_TX0 +chn_offset, McSPI_Module[McSPI_ID].CH[chn].TxRx.Tx );
				}
				else {	/* must set a dummy data in Tx reigster of receive only mode .*/
					write_reg(mcspi_ptr[McSPI_ID], MCSPI_TX0 +chn_offset, 0);
				}

				/* waiting for EOT ,not support interrupt yet , using polling waiting for the moment*/
				reg_value =0;
				while(!MCSPI_GET_CHSTAT_EOT(reg_value)) {
					reg_value = read_reg(mcspi_ptr[McSPI_ID], MCSPI_CH0STAT + chn_offset);
					sched_yield();
				}
                                while(!MCSPI_GET_CHSTAT_RXS(reg_value)) {
					reg_value = read_reg(mcspi_ptr[McSPI_ID], MCSPI_CH0STAT + chn_offset);
					sched_yield();
				}

				/* copy receive data */
				if(McSPI_Module[McSPI_ID].CH[chn].TxRx.flag & McSPI_TxRx_Rx) {
					McSPI_Module[McSPI_ID].CH[chn].TxRx.Rx = read_reg(mcspi_ptr[McSPI_ID], MCSPI_RX0 + chn_offset);
				}

				/* channel disable */
				write_reg(mcspi_ptr[McSPI_ID], MCSPI_CH0CTRL + chn_offset, 0);
			}
		}
	}
	else {
#ifdef BBBIO_LIB_DBG
		printf("BBBIO_McSPI_work: McSPI %d CM_PER Clock Gating!\n",McSPI_ID);
#endif
		return 0 ;
	}
	return 1 ;
}
/* ----------------------------------------------------------------------------------------------- */
int BBBIO_McSPI_Setting(unsigned int McSPI_ID ,
			unsigned int channel,
			unsigned int MS, 		/* MS 			,maset or slave */
			unsigned int TRM ,		/* TRM 			,Tx only / Rx only , TxRx */
			unsigned int CLK_div ,		/* clock divider	, default clock : 48M Hz */
			unsigned int CLKmode ,		/* POL/PHA ,		,clock polarity */
			unsigned int EPOL ,
			unsigned int DataDir ,		/* IS/DPE1/DPE0		,data0 Output data1 Input ,or data0 Input data1 Output */
			unsigned int WL)		/* WL 			,word length */
{
	volatile unsigned int* reg;
	unsigned int reg_value ;
	unsigned int chn_offset =channel *0x14;

	/* check CM_PER enable status, or it may caue "Bus error" signal message. */
        if(McSPI_Module[McSPI_ID].CM_PER_enable) {
		/* disable channel */
		write_reg(mcspi_ptr[McSPI_ID], MCSPI_CH0CTRL + chn_offset, 0);
		write_reg(mcspi_ptr[McSPI_ID], MCSPI_MODULCTRL, MS << 2);
 		write_reg(mcspi_ptr[McSPI_ID], MCSPI_CH0CONF + chn_offset, (DataDir << 16 | TRM << 12 | (WL - 1) << 7 | EPOL << 6 | CLK_div << 2 | CLKmode));

		if(TRM != BBBIO_McSPI_Tx_Only)
			McSPI_Module[McSPI_ID].CH[channel].TxRx.flag |= McSPI_TxRx_Rx ;
	        if(TRM != BBBIO_McSPI_Rx_Only) {
        	        McSPI_Module[McSPI_ID].CH[channel].TxRx.flag |= McSPI_TxRx_Tx ;
			McSPI_Module[McSPI_ID].CH[channel].TxRx.Tx =0;
		}
		McSPI_Module[McSPI_ID].CH[channel].status |=McSPI_CH_ENABLE;
	}
        else {
#ifdef BBBIO_LIB_DBG
		printf("BBBIO_McSPI_work: McSPI %d CM_PER Clock Gating!\n",McSPI_ID);
#endif
                return 0 ;
        }
	return 1 ;
}
/* ----------------------------------------------------------------------------------------------- */
/* Beaglebone Black , SPI Expansion Header Mode Check */
int BBBIO_McSPI_EP_check(unsigned int McSPI_ID)
{
	unsigned int ret = 1;

	if(McSPI_ID ==1) {
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
//#define BBBIO_McSPI_Enable(a) BBBIO_McSPI_CLK_set(a,1,0)
//#define BBBIO_McSPI_Disable(a) BBBIO_McSPI_CLK_set(a,0,0)

int BBBIO_McSPI_CLK_set(unsigned int McSPI_ID ,int enable , int idle)
{
	volatile unsigned int* reg = NULL;
	const unsigned int CM_PER_McSPI[] = {BBBIO_CM_PER_SPI0_CLKCTRL, BBBIO_CM_PER_SPI1_CLKCTRL};

	if(McSPI_ID > 1 || McSPI_ID < 0) {
#ifdef BBBIO_LIB_DBG
		printf("BBBIO_McSPI_CLK_set: Unknow McSPI module ID : %d\n", McSPI_ID);
#endif
		return 0 ;
	}

	/* pin mux check */
	if(!BBBIO_McSPI_EP_check(McSPI_ID)) {
#ifdef BBBIO_LIB_DBG
		printf("BBBIO_McSPI_CLK_set: No effect pin mux Detected of SPI-%d ,Please use EP_STATUS toolkit to check Pin mux mode\n", McSPI_ID);
#endif
		return 0;
	}

	if(enable) {
		write_reg(cm_per_addr, CM_PER_McSPI[McSPI_ID], 0x3 << 16 | 0x2);
		McSPI_Module[McSPI_ID].CM_PER_enable = 1;
	}
	else {
		write_reg(cm_per_addr, CM_PER_McSPI[McSPI_ID], 0);
		McSPI_Module[McSPI_ID].CM_PER_enable = 0;
	}

	return 1 ;
}
/* ----------------------------------------------------------------------------------------------- */
void BBBIO_McSPI_Reset(unsigned int McSPI_ID)
{
	unsigned int reg_value ;

	/* reset */
	write_reg(mcspi_ptr[McSPI_ID], MCSPI_SYSCONFIG, 0x2 << 3 | 0x2);

	/* waiting reset finish */
	reg_value =0;
	while(!MCSPI_GET_SYSSTATUS_RESETDONE(reg_value)) {
		reg_value = read_reg(mcspi_ptr[McSPI_ID] ,MCSPI_SYSSTATUS);
		sched_yield();
	}
#ifdef BBBIO_LIB_DBG
	printf("Reset Finish\n");
#endif
}
