#ifndef BBBIO_MSCPI_H
#define BBBIO_MSCPI_H
//----------------------------------------------------------------------------
#define BBBIO_McSPI_SPI0	0
#define BBBIO_McSPI_SPI1	1

#define BBBIO_McSPI_CH0		0
#define BBBIO_McSPI_CH1		1
#define BBBIO_McSPI_CH2		2	/* CH2 & CH3 not pin out , useless */
#define BBBIO_McSPI_CH3		3

/* Rerence AM335x Techinal Reference Manual .
 *	Session 24.3.1.1 , 24.3.1.2.1 , 24.3.1.2.2
 */
#define BBBIO_McSPI_Master	0x0
#define BBBIO_McSPI_Slave	0x1

#define BBBIO_McSPI_TxRx	0x0
#define BBBIO_McSPI_Rx_Only	0x1
#define BBBIO_McSPI_Tx_Only	0x2

/* Reference AM335x Techinal Reference Manual .
 * 	Session 24.3.1.3.6 Polarity and Phase
 */
#define BBBIO_McSPI_CLK_MODE0	0x0
#define BBBIO_McSPI_CLK_MODE1	0x1
#define BBBIO_McSPI_CLK_MODE2	0x2
#define BBBIO_McSPI_CLK_MODE3	0x3


/* Reference AM335x Techinal Reference Manual .
 *	page :4594 , bit 16~18 , Field : IS , DPE1 , DPE0
 */
#define BBBIO_McSPI_D0O_D1I	0x6
#define BBBIO_McSPI_D0I_D1O	0x1

#define BBBIO_McSPI_EN_ACT_HIGH	0x0
#define BBBIO_McSPI_EN_ACT_LOW	0x1
//-------------------------------------------------------------------------
int BBBIO_McSPI_work(unsigned int McSPI_ID);

int BBBIO_McSPI_Init() ;

void BBBIO_McSPI_Tx(unsigned int McSPI_ID ,unsigned int channel ,unsigned int data) ;

unsigned int BBBIO_McSPI_Rx(unsigned int McSPI_ID ,unsigned int channel ) ;

#define BBBIO_McSPI_Enable(a) BBBIO_McSPI_CLK_set(a,1,0)
#define BBBIO_McSPI_Disable(a) BBBIO_McSPI_CLK_set(a,0,0)
int BBBIO_McSPI_CLK_set(unsigned int McSPI_ID ,int enable , int idle) ;

int BBBIO_McSPI_Setting(unsigned int McSPI_ID ,
                        unsigned int channel,
                        unsigned int MS,		/* MS		 , maset or slave */
                        unsigned int TRM ,		/* TRM		 , Tx only / Rx only , TxRx */
                        unsigned int CLK_div ,		/* Clock Divider , default clock : 48M Hz */
                        unsigned int CLKmode ,		/* POL/PHA ,     , clock polarity */
                        unsigned int EPOL ,
                        unsigned int DataDir ,		/* IS/DPE1/DPE0	 ,data0 Outpu    t data1 Input ,or data0 Input data1 Output */
                        unsigned int WL);		// WL		 ,word length */

void BBBIO_McSPI_Reset(unsigned int McSPI_ID);

//-------------------------------------------------------------------------
#endif
