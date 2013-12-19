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
#define BBBIO_MCSPI_ARG_MODULE_COUNT				2
#define BBBIO_MCSPI_ARG_CHANNEL_COUNT				2 // AM335x have 4channel , but only pinout2

// Device register mamory map
#define BBBIO_MCSPI0_ADDR				0x48030000
#define BBBIO_MCSPI1_ADDR				0x481A0000
#define BBBIO_MCSPI_LEN					0x1000

// Device register offset
#define BBBIO_MCSPI_SYSCONFIG           0x110
#define BBBIO_MCSPI_IRQENABLE           0x11c
#define BBBIO_MCSPI_IRQSTATUS           0x118
#define BBBIO_MCSPI_SYSSTATUS           0x114
#define BBBIO_MCSPI_SYST                0x124
#define BBBIO_MCSPI_MODULCTRL           0x128
#define BBBIO_MCSPI_CH0CONF             0x12C
#define BBBIO_MCSPI_CH0STAT             0x130
#define BBBIO_MCSPI_CH0CTRL             0x134
#define BBBIO_MCSPI_TX0                 0x138
#define BBBIO_MCSPI_RX0                 0x13C
#define BBBIO_MCSPI_XFERLEVEL           0x17C
#define BBBIO_MCSPI_DAFTX               0x180
#define BBBIO_MCSPI_DAFRX               0x1A0


#define BBBIO_MCSPI_CH_REG_OFFSET		0x14		// offset between MCSPI_CH0CONF / MCSPI_CH1CONF /MCSPI_CH2CONF /MCSPI_CH3CONF

#define BBBIO_CM_PER_SPI1_CLKCTRL_IDLEST_MASK	(0x3 <<16)
#define BBBIO_MCSPI_GET_CHSTAT_EOT(a)			((a&0x04)>>2)
//-----------------------------------------------------------------------------------------------

#define McSPI_TxRx_Tx		0x01
#define McSPI_TxRx_Rx		0x02

struct BBBIO_McSPI_TxRx_struct
{
	unsigned int Tx ;
	unsigned int Rx ;
	unsigned int flag ;
};

// Channel struct
#define McSPI_CH_IDLE	0x1
#define McSPI_CH_WOT	0x2		// waiting of transmit
#define McSPI_CH_EOT	0x4		// end of transmit

struct BBBIO_McSPI_CH_struct
{
	int status ;
	unsigned int CHCONF;
	unsigned int CHSTAT;
	unsigned int CHCTRL;
	struct BBBIO_McSPI_TxRx_struct TxRx ;
};

// McSPI module struct
struct BBBIO_McSPI_struct
{
	int CM_PER_enable ;
	unsigned int SYSCONFIG ;
	unsigned int IRQENABLE ;
	unsigned int IRQSTATUS ;
	unsigned int SYSSTATUS ;
	unsigned int MODULCTRL ;
	struct BBBIO_McSPI_CH_struct CH[BBBIO_MCSPI_ARG_CHANNEL_COUNT];	// AM335x have 4channel , but only pinout2
};
//-----------------------------------------------------------------------------------------------
extern int memh;
extern volatile unsigned int *cm_per_addr ;
const unsigned int McSPI_AddressOffset[]={BBBIO_MCSPI0_ADDR,
                                          BBBIO_MCSPI1_ADDR};
volatile unsigned int *mcspi_ptr[BBBIO_MCSPI_ARG_CHANNEL_COUNT] ={NULL};

struct BBBIO_McSPI_struct McSPI_Module[BBBIO_MCSPI_ARG_MODULE_COUNT] ={0} ;
//-----------------------------------------------------------------------------------------------
inline void write_reg(volatile void *reg_base ,unsigned int offset ,unsigned int data)
{
	*((volatile unsigned int* )(reg_base+offset)) = data ;
}
//-----------------------------------------------------------------------------------------------
inline unsigned int read_reg(volatile void *reg_base ,unsigned int offset)
{
	return *((volatile unsigned int* )(reg_base+offset));
}
//-----------------------------------------------------------------------------------------------
/*********************************
 McSPI init
 *******************************
 * iolib_init() will run this function automatically
 *
 *      @return         : 1 for success , 0 for failed
 */
int BBBIO_McSPI_Init()
{
    int i ;
    if (memh ==0)
    {
        if (BBBIO_LIB_DBG) printf("BBBIO_PWM_Init: memory not mapped?\n");
            return 0;
    }
    for (i=0; i<2; i++)
    {
        mcspi_ptr[i] = mmap(0 ,BBBIO_MCSPI_LEN ,PROT_READ | PROT_WRITE ,MAP_SHARED ,memh ,McSPI_AddressOffset[i]);
        if(mcspi_ptr[i] == MAP_FAILED)
        {
            if (BBBIO_LIB_DBG) printf("BBBIO_McSPI_Init: McSPI %d mmap failure!\n",i);
                return 0;
        }
    }
    return 1;
}
//-----------------------------------------------------------------------------------------------
// Transmit data
void BBBIO_McSPI_Tx(unsigned int McSPI_ID ,unsigned int channel ,unsigned int data)
{
	McSPI_Module[McSPI_ID].CH[channel].TxRx.Tx =data;
}
//-----------------------------------------------------------------------------------------------
// Receive data
unsigned int BBBIO_McSPI_Rx(unsigned int McSPI_ID ,unsigned int channel )
{
	return McSPI_Module[McSPI_ID].CH[channel].TxRx.Rx ;
}
//-----------------------------------------------------------------------------------------------
// McSPI module work
int BBBIO_McSPI_work(unsigned int McSPI_ID)
{
	unsigned int chn ;
	unsigned int chn_offset =0;
	unsigned int reg_value ;

	if(McSPI_Module[McSPI_ID].CM_PER_enable)	// check CM_PER enable status, or it may caue "Bus error" signal message.
	{
		struct BBBIO_McSPI_CH_struct *CH_ptr = &McSPI_Module[McSPI_ID].CH[0] ;

		for(chn=0 ; chn<BBBIO_MCSPI_ARG_CHANNEL_COUNT ; chn++)
		{
			chn_offset = chn * 0x14 ;

			write_reg(mcspi_ptr[McSPI_ID] ,
					  BBBIO_MCSPI_CH0CTRL + chn_offset ,
					  1);// channel enable

			// need transform data
			if(McSPI_Module[McSPI_ID].CH[chn].TxRx.flag & McSPI_TxRx_Tx)
			{
				write_reg(mcspi_ptr[McSPI_ID] ,
						  BBBIO_MCSPI_TX0 +chn_offset ,
						  McSPI_Module[McSPI_ID].CH[chn].TxRx.Tx );
			}
			else	// must set a dummy data in Tx reigster of receive only mode .
			{
				write_reg(mcspi_ptr[McSPI_ID] ,
						  BBBIO_MCSPI_TX0 +chn_offset ,
						  0);
			}
			// waiting for EOT ,not support interrupt yet , so it will cause waiting
			reg_value =0;
			while( !BBBIO_MCSPI_GET_CHSTAT_EOT(reg_value))
			{
				reg_value = read_reg(mcspi_ptr[McSPI_ID] ,BBBIO_MCSPI_CH0STAT +chn_offset);
				sched_yield();
			}

			// need receive data
			if(McSPI_Module[McSPI_ID].CH[chn].TxRx.flag & McSPI_TxRx_Rx)
			{
				McSPI_Module[McSPI_ID].CH[chn].TxRx.Rx = read_reg(mcspi_ptr[McSPI_ID] ,BBBIO_MCSPI_RX0 +chn_offset);
			}

			write_reg(mcspi_ptr[McSPI_ID] ,
					  BBBIO_MCSPI_CH0CTRL + chn_offset ,
					  0);   // channel disable
		}
	}
	else
		return 0 ;

	return 1 ;
}
//-----------------------------------------------------------------------------------------------
int BBBIO_McSPI_Setting(unsigned int McSPI_ID ,
			unsigned int channel,
			unsigned int MS, 		// MS 			,maset or slave
			unsigned int TRM ,		// TRM 			,Tx only / Rx only , TxRx
			unsigned int CLK_div ,		// clock divider	, default clock : 48M Hz
			unsigned int CLKmode ,		// POL/PHA ,		,clock polarity
			unsigned int EPOL ,
			unsigned int DataDir ,		// IS/DPE1/DPE0		,data0 Outpu	t data1 Input ,or data0 Input data1 Output
			unsigned int WL) 			// WL 		,word length
{
	volatile unsigned int* reg;
	unsigned int reg_value ;
	unsigned int chn_offset =channel *0x14;

	write_reg(mcspi_ptr[McSPI_ID] ,
			  BBBIO_MCSPI_CH0CTRL + chn_offset ,
			  0);   // channel disable       

	write_reg(mcspi_ptr[McSPI_ID] ,
			  BBBIO_MCSPI_MODULCTRL ,
			  MS <<2);   

	write_reg(mcspi_ptr[McSPI_ID] ,
			  BBBIO_MCSPI_CH0CONF + chn_offset ,
			  DataDir <<16 | TRM <<12 | (WL-1) << 7 | EPOL<<6 | CLK_div <<2 | CLKmode); 

	if(TRM != BBBIO_McSPI_Tx_Only)
		McSPI_Module[McSPI_ID].CH[channel].TxRx.flag |= McSPI_TxRx_Rx ;


}
//-----------------------------------------------------------------------------------------------
#define BBBIO_McSPI_Enable(a) BBBIO_McSPI_CLK_set(a,1,0)
#define BBBIO_McSPI_Disable(a) BBBIO_McSPI_CLK_set(a,0,0)

int BBBIO_McSPI_CLK_set(unsigned int McSPI_ID ,int enable , int idle)
{
	volatile unsigned int* reg =NULL;
	const unsigned int CM_PER_McSPI[]={BBBIO_CM_PER_SPI0_CLKCTRL,
                                       BBBIO_CM_PER_SPI1_CLKCTRL};
	if(McSPI_ID >1 || McSPI_ID <0)
	{
		if (BBBIO_LIB_DBG) printf("BBBIO_McSPI_CLK_set: McSPI Module %d setting error\n",McSPI_ID);
		return 0 ;
	}

	reg=(void*)cm_per_addr + CM_PER_McSPI[McSPI_ID] ;

	if(enable)
		*reg= 2;		// enable 
	else
		*reg= 0;		// disable

	McSPI_Module[McSPI_ID].CM_PER_enable = *reg & ~BBBIO_CM_PER_SPI1_CLKCTRL_IDLEST_MASK; 
	if (BBBIO_LIB_DBG) printf("BBBIO_McSPI_CLK_set: McSPI Module %d setting finish\n",McSPI_ID);

	return 1 ;
}
//-----------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------
