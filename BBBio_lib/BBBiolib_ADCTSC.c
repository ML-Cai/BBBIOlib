/*
 * This library is a crude ADC library for Beaglebone black .
 *
 * support "Single Channel Single Step" ADC sample control  ,  not support Interrupt yet ,
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sched.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include "BBBiolib.h"
#include "BBBiolib_ADCTSC.h"
#include <unistd.h>
#include <signal.h>

#include <sys/time.h>
//-----------------------------------------------------------------------------------------------
/* Argument define */

/* Beaglebone black ADC have 7 AIN (0~6) */
#define ADCTSC_AIN_COUNT	7


/* Device register mamory map */
#define ADCTSC_MMAP_ADDR	0x44E0D000
#define ADCTSC_MMAP_LEN	0x2000

/* Device register offset */
#define ADCTSC_REVISION	0x0
#define ADCTSC_SYSCONFIG	0x10
#define ADCTSC_IRQSTATUS_RAW	0x24
#define ADCTSC_IRQSTATUS	0x28
#define ADCTSC_IRQENABLE_SET	0x2C
#define ADCTSC_IRQENABLE_CLR	0x30
#define ADCTSC_IRQWAKEUP	0x34
#define ADCTSC_DMAENABLE_SET	0x38
#define ADCTSC_DMAENABLE_CLR	0x3C
#define ADCTSC_CTRL	0x40
#define ADCTSC_ADCSTAT	0x44
#define ADCTSC_ADCRANGE	0x48
#define ADCTSC_ADC_CLKDIV	0x4C
#define ADCTSC_ADC_MISC	0x50
#define ADCTSC_STEPENABLE	0x54
#define ADCTSC_IDLECONFIG	0x58
#define ADCTSC_TS_CHARGE_STEPCONFIG	0x5C
#define ADCTSC_TS_CHARGE_DELAY	0x60
#define ADCTSC_STEPCONFIG1	0x64
#define ADCTSC_STEPDELAY1	0x68
#define ADCTSC_STEPCONFIG2	0x6C
#define ADCTSC_STEPDELAY2	0x70
#define ADCTSC_STEPCONFIG3	0x74
#define ADCTSC_STEPDELAY3	0x78
#define ADCTSC_STEPCONFIG4	0x7C
#define ADCTSC_STEPDELAY4	0x80
#define ADCTSC_STEPCONFIG5	0x84
#define ADCTSC_STEPDELAY5	0x88
#define ADCTSC_STEPCONFIG6	0x8C
#define ADCTSC_STEPDELAY6	0x90
#define ADCTSC_STEPCONFIG7	0x94
#define ADCTSC_STEPDELAY7	0x98
#define ADCTSC_STEPCONFIG8	0x9C
#define ADCTSC_STEPDELAY8	0xA0
#define ADCTSC_STEPCONFIG9	0xA4
#define ADCTSC_STEPDELAY9	0xA8
#define ADCTSC_STEPCONFIG10	0xAC
#define ADCTSC_STEPDELAY10	0xB0
#define ADCTSC_STEPCONFIG11	0xB4
#define ADCTSC_STEPDELAY11	0xB8
#define ADCTSC_STEPCONFIG12	0xBC
#define ADCTSC_STEPDELAY12	0xC0
#define ADCTSC_STEPCONFIG13	0xC4
#define ADCTSC_STEPDELAY13	0xC8
#define ADCTSC_STEPCONFIG14	0xCC
#define ADCTSC_STEPDELAY14	0xD0
#define ADCTSC_STEPCONFIG15	0xD4
#define ADCTSC_STEPDELAY15	0xD8
#define ADCTSC_STEPCONFIG16	0xDC
#define ADCTSC_STEPDELAY16	0xE0
#define ADCTSC_FIFO0COUNT	0xE4
#define ADCTSC_FIFO0THRESHOLD	0xE8
#define ADCTSC_DMA0REQ	0xEC
#define ADCTSC_FIFO1COUNT	0xF0
#define ADCTSC_FIFO1THRESHOLD	0xF4
#define ADCTSC_DMA1REQ	0xF8
#define ADCTSC_FIFO0DATA	0x100
#define ADCTSC_FIFO1DATA	0x200

/* ADCRANGE operator code */
#define ADCRANGE_MAX_RANGE	0xFFF
#define ADCRANGE_MIN_RANGE	0x000

/* CTRL operator code */
#define CTRL_ENABLE	0x1
#define CTRL_STEP_ID_TAG	0x2

/* ----------------------------------------------------------------------------------------------- */
/* struct definition */
struct ADCTSC_FIFO_struct
{
	unsigned int *reg_count ;
	unsigned int *reg_data ;
	struct ADCTSC_FIFO_struct *next ;
};

struct ADCTSC_channel_struct
{
	unsigned int enable ;	 /* HW channel en/disable,  */
	unsigned int mode ;
	unsigned int FIFO ;
	/*  13 + O + S cycle per sample, 13 is ADC converting time, O is open delay, S is sample delay .
	 *	Open delay  minmum : 0
	 *	Sample delay  minmum : 1
	 */
	unsigned int delay ;	/* bit 0~17 open delay , bit 24~31 sample delay  */

	/* channel buffer */
	unsigned int *buffer ;
	unsigned int buffer_size ;
	unsigned int buffer_count ;
	unsigned int *buffer_save_ptr ;
};

struct ADCTSC_struct
{
	unsigned int work_mode ;
	unsigned int H_range;
	unsigned int L_range;
	unsigned int ClockDiv;	/* Clock divider , Default ADC clock :24MHz */
	struct ADCTSC_channel_struct channel[8];
	struct ADCTSC_FIFO_struct FIFO[2] ;
	unsigned char channel_en ;	/* Const SW channel en/disable, not real channel en/disable */
	unsigned char channel_en_var;	/* Variable  channel SW enable , for access */
	int fetch_size;
};

/* ----------------------------------------------------------------------------------------------- */
/* Global Variable */
extern int memh;
extern volatile unsigned int *cm_wkup_addr;
volatile unsigned int *adctsc_ptr = NULL;

struct ADCTSC_struct ADCTSC ;
/* ----------------------------------------------------------------------------------------------- */
/* ADCTSC set range
 *
 * set ADC step sample max range and min range , Max range : 4095 (1.8v) , Min range : 0 (0v)
 *
 * 	@param L_range : Min range .
 *      @param H_range : Max range .
 *
 *	@return : 0 for error , 1 for success .
 *
 * 	@Note : Max Voltage in ADC_TSC is 1.8v .
 *		L_range and H_range compare with ADC data , if the sampled data is less(L) /greater(H) than the value,
 *		a interrupt will generated . BUT , no interrup process in this library ,just ignore the inconformity data (not store in FIFO).
 *
 */
static int BBBIO_ADCTSC_set_range(int L_range, int H_range)
{
	unsigned int *reg = NULL;

	if((L_range > 4095) || (L_range < 0) || (H_range > 4095) || (H_range < 0) || (H_range < L_range)) {
#ifdef BBBIO_LIB_DBG
		fprintf(stderr, "BBBIO_ADCTSC_set_range : ADC range error : [L:%d L ,H:%d] , (0 <= range <= 4095)\n", L_range, H_range);
#endif
		return 0 ;
	}

	reg = (void *)adctsc_ptr + ADCTSC_ADCRANGE;
	*reg |= (L_range | H_range << 16) ;
	return 1 ;
}

/* ----------------------------------------------------------------------------------------------- */
/* ADCTSC Channel status controller
 *
 *	#define BBBIO_ADCTSC_channel_enable(A) BBBIO_ADCTSC_channel_status(A,1)
 *	#define BBBIO_ADCTSC_channel_disable(A)	BBBIO_ADCTSC_channel_status(A,0)
 */
int BBBIO_ADCTSC_channel_status(int chn_ID ,int enable)
{
	unsigned int *reg = NULL;

	if((chn_ID < 0) || (chn_ID > 6)) {
#ifdef BBBIO_LIB_DBG
		fprintf(stderr, "BBBIO_ADCTSC_Channel_status : Channel ID error [%d]\n", chn_ID);
#endif
		return 0;
	}
	else {
		/* step enable */
		if(enable) {
			ADCTSC.channel[chn_ID].enable = 1;
			reg = (void *)adctsc_ptr + ADCTSC_STEPENABLE;
			*reg |= 0x0001 << (chn_ID+1);
		}
		else {
			ADCTSC.channel[chn_ID].enable = 0;
			reg = (void *)adctsc_ptr + ADCTSC_STEPENABLE;
			*reg &= ~(0x0001 << (chn_ID+1));
		}

		/* Reset buffer counter*/
		ADCTSC.channel[chn_ID].buffer_count = 0;
		ADCTSC.channel[chn_ID].buffer_save_ptr = ADCTSC.channel[chn_ID].buffer;
	}
	return 1;
}

/* ----------------------------------------------------------------------------------------------- */
/* ADCTSC module control
 *
 * control module status .
 *
 *	@param work_type : ADC work type ,Busy polling or Timer interrupt.
 *      @param clkdiv : ADC_TSC clock divider , (default ADC module Clock : 24MHz).
 *
 */
void BBBIO_ADCTSC_module_ctrl(unsigned int work_type, unsigned int clkdiv)
{
	unsigned int *reg = NULL;

	if((clkdiv < 1) || (clkdiv > 65535)) {
#ifdef BBBIO_LIB_DBG
		fprintf(stderr, "BBBIO_ADCTSC_module_ctrl : Clock Divider value error [%d]\n");
#endif
		clkdiv = 1;
	}
	else {
		reg = (void *)adctsc_ptr + ADCTSC_ADC_CLKDIV;
		*reg = (clkdiv -1) ;
	}

	if((work_type == BBBIO_ADC_WORK_MODE_BUSY_POLLING) || (work_type == BBBIO_ADC_WORK_MODE_TIMER_INT)) {
		ADCTSC.work_mode = work_type;
	}
	else {
		fprintf(stderr, "BBBIO_ADCTSC_module_ctrl : Work Type setting error\n");
	}
}
/* ----------------------------------------------------------------------------------------------- */
/* ADCTSC channel buffering
 *
 * assian new buffer for channel x , this function is desianed for multi data store application to avoid memcpy .
 *
 *	@param chn_ID : channel ID which need configure. (BBBIO_AIN0 ~ BBBIO_AIN6)
 *	@param buf : buffer for store data.
 *	@param buf_size : buffer size .
 *
*/
int BBBIO_ADCTSC_channel_buffering(unsigned int chn_ID, unsigned int *buf, unsigned int buf_size) 
{
	/* assian buffer */
	if(buf != NULL && buf_size > 0) {
		ADCTSC.channel[chn_ID].buffer = buf;
		ADCTSC.channel[chn_ID].buffer_size = buf_size;
		ADCTSC.channel[chn_ID].buffer_save_ptr = buf;
		ADCTSC.channel[chn_ID].buffer_count = 0;
		ADCTSC.channel_en |= 1 << chn_ID;	/* SW channel enable */
		return 1;
	}
	else {
		ADCTSC.channel_en &= ~(1 << chn_ID);	/* SW channel disable */
		return 0;
	}
}

/* ----------------------------------------------------------------------------------------------- */
/* ADCTSC channel control
 *
 * control each channel sample status . each chnnel mapped one step .
 *
 *	@param chn_ID : channel ID which need configure. (BBBIO_AIN0 ~ BBBIO_AIN6)
 *	@param mode : sample mode ,one-shot or continus. (SW mode only , HW synchronized not implement)
 *	@param sample_avg : Number of samplings to average. (BBBIO_ADC_STEP_AVG BBBIO_ADC_STEP_AVG_1, 2, 4, 8, 16)
 *	@param open_dly : open delay ,default :0 , max :262143 .
 *	@param sample_dly : sample delat , default :1 , max :255 .
 *	@param buf : buffer for store data.
 *	@param buf_size : buffer size.
 *
 */
int BBBIO_ADCTSC_channel_ctrl(unsigned int chn_ID, int mode, int open_dly, int sample_dly, int sample_avg, unsigned int *buf, unsigned int buf_size)
{
	unsigned int *reg = NULL;

	if((chn_ID > BBBIO_ADC_AIN6) || (chn_ID < BBBIO_ADC_AIN0) ||
	   (sample_avg > BBBIO_ADC_STEP_AVG_16) || (sample_avg < BBBIO_ADC_STEP_AVG_1) ||
	   (open_dly > 262143) || (open_dly < 0) || (sample_dly > 255) || (sample_dly < 1)){
#ifdef BBBIO_LIB_DBG
		fprintf(stderr, "BBBIO_ADCTSC_channel_ctrl : argument error\n");
		return 0;
#endif

	}



	/* assian buffer */
	if(buf != NULL && buf_size > 0) {
		ADCTSC.channel[chn_ID].buffer = buf;
		ADCTSC.channel[chn_ID].buffer_size = buf_size;
		ADCTSC.channel[chn_ID].buffer_save_ptr = buf;
		ADCTSC.channel[chn_ID].buffer_count = 0;
		ADCTSC.channel_en |= 1 << chn_ID;	/* SW enable */
	}
	else {
		ADCTSC.channel_en &= ~(1 << chn_ID);	/* SW disable */
	}

	/* Disable channel step*/
	BBBIO_ADCTSC_channel_disable(chn_ID);

	/* cancel step config register protection*/
	reg = (void *)adctsc_ptr + ADCTSC_CTRL;
	*reg |= 0x4 ;

	/* set step config */
	reg = (void *)adctsc_ptr + (ADCTSC_STEPCONFIG1 + chn_ID * 0x8);
	*reg &= ~(0x1F) ;	/* pre-maks Mode filed */
	*reg |= (mode | (sample_avg << 2) | (chn_ID << 19) | (chn_ID << 15) | ((chn_ID % 2) << 26) );

	/* set open delay */
	if(open_dly <0 || open_dly >262143) {
		open_dly = 0;
	}
	reg = (void *)adctsc_ptr + (ADCTSC_STEPDELAY1 + chn_ID * 0x8);
	*reg =0;
	*reg |= ((sample_dly - 1) << 24 | open_dly);

	/* resume step config register protection*/
	reg = (void *)adctsc_ptr + ADCTSC_CTRL;
	*reg &= ~0x4 ;

	return 1;
}
/* ----------------------------------------------------------------------------------------------- */
/* signal function for BBBIO_ADCTSC_work (sig SIGALRM)
 *
 */
static void _ADCTSC_work(int sig_arg)
{
	unsigned int *reg_count = NULL;
	unsigned int *reg_data = NULL;
	unsigned int buf_data = 0;
	int FIFO_count = 0;
	int chn_ID =0;
	struct ADCTSC_channel_struct *chn_ptr =NULL;
	struct ADCTSC_FIFO_struct *FIFO_ptr = ADCTSC.FIFO;
	int i ,j;

	/* waiting FIFO buffer fetch a data*/
	for(j = 0 ; j < 2 ; j++) {
		reg_count = FIFO_ptr->reg_count;
		reg_data = FIFO_ptr->reg_data;

		FIFO_count = *reg_count;
		if(FIFO_count > 0) {
			/* fetch data from FIFO */
			for(i = 0 ; i < FIFO_count ; i++) {
				buf_data = *reg_data;
				chn_ID = (buf_data >> 16) & 0xF;
				chn_ptr = &ADCTSC.channel[chn_ID];

				if((chn_ptr->buffer_size > chn_ptr->buffer_count) && (ADCTSC.fetch_size > chn_ptr->buffer_count)) {
					*(chn_ptr->buffer_save_ptr) = buf_data & 0xFFF;
					chn_ptr->buffer_save_ptr++;
					chn_ptr->buffer_count ++;
				}
				else {
					ADCTSC.channel_en_var &= ~(1 << chn_ID);	/* SW Disable this channel */
					/* No break here , still work for clear fifo */
				}
			}
		}
		/* switch to next FIFO */
		FIFO_ptr = FIFO_ptr->next;
	}
}
/* ----------------------------------------------------------------------------------------------- */
/* ADCTSC fetch data
 *
 *	fetch a word from ADC . this function is blocking function .
 *
 *
 */
unsigned int BBBIO_ADCTSC_work(unsigned int fetch_size)
{
	unsigned int *reg_count = NULL;
	unsigned int *reg_data = NULL;
	unsigned int *reg_ctrl = NULL;
	unsigned int buf_data = 0;
	int FIFO_count = 0;
	int chn_ID =0;
	struct ADCTSC_channel_struct *chn_ptr =NULL;
	struct ADCTSC_FIFO_struct *FIFO_ptr = ADCTSC.FIFO;
	int i ;
//	unsigned int tmp_channel_en = ADCTSC.channel_en;

	/* Start sample */
	for(chn_ID = 0 ; chn_ID < ADCTSC_AIN_COUNT ; chn_ID++) {
		if(ADCTSC.channel_en & (1 << chn_ID)) {
			ADCTSC.channel[chn_ID].buffer_save_ptr =ADCTSC.channel[chn_ID].buffer; /* re-pointer save pointer */
			BBBIO_ADCTSC_channel_enable(chn_ID);
		}
	}

	/* Enable module and tag channel ID in FIFO data*/
	reg_ctrl = (void *)adctsc_ptr + ADCTSC_CTRL;
	*reg_ctrl |= (CTRL_ENABLE | CTRL_STEP_ID_TAG);

	ADCTSC.fetch_size = fetch_size;
	ADCTSC.channel_en_var = ADCTSC.channel_en;

	if(ADCTSC.work_mode & BBBIO_ADC_WORK_MODE_TIMER_INT) {
		struct itimerval ADC_t;
		ADC_t.it_interval.tv_usec = 200;
		ADC_t.it_interval.tv_sec = 0;
		ADC_t.it_value.tv_usec = 200;
		ADC_t.it_value.tv_sec = 0;

		signal(SIGALRM, _ADCTSC_work);
		if(setitimer( ITIMER_REAL, &ADC_t, NULL) < 0 ){
			printf("setitimer error\n");
			return 0;
		}
		while(ADCTSC.channel_en_var !=0) {
			usleep(10000);
		}
		ADC_t.it_interval.tv_usec = 0;
		ADC_t.it_interval.tv_sec = 0;
		ADC_t.it_value.tv_usec = 0;
		ADC_t.it_value.tv_sec = 0;
		setitimer( ITIMER_REAL, &ADC_t, NULL);
		signal(SIGALRM, NULL);
	}
	else { /* Busy Polling mode */
		/* waiting FIFO buffer fetch a data */
		while(ADCTSC.channel_en_var !=0) {
			reg_count = FIFO_ptr->reg_count;
			reg_data = FIFO_ptr->reg_data;

			FIFO_count = *reg_count;
			if(FIFO_count > 0) {
				/* fetch data from FIFO */
				for(i = 0 ; i < FIFO_count ; i++) {
					buf_data = *reg_data;
					chn_ID = (buf_data >> 16) & 0xF;
					chn_ptr = &ADCTSC.channel[chn_ID];
	
					if((chn_ptr->buffer_size > chn_ptr->buffer_count) && (fetch_size > chn_ptr->buffer_count)) {
						*(chn_ptr->buffer_save_ptr) = buf_data & 0xFFF;
						chn_ptr->buffer_save_ptr++;
						chn_ptr->buffer_count ++;
					}
					else {
						ADCTSC.channel_en_var &= ~(1 << chn_ID);	// SW Disable this channel 
					}
				}
//				tv.tv_sec = 0;
//				tv.tv_usec = 40;
//				select(0, NULL, NULL, NULL, &tv);
			}
			// switch to next FIFO 
			FIFO_ptr = FIFO_ptr->next;
		}
	}

	/* all sample finish */
        for(chn_ID = 0 ; chn_ID < ADCTSC_AIN_COUNT ; chn_ID++) {
		if(ADCTSC.channel_en & (1 << chn_ID)) {
			BBBIO_ADCTSC_channel_disable(chn_ID);
		}
        }

	reg_ctrl = (void *)adctsc_ptr + ADCTSC_CTRL;
        *reg_ctrl &= ~(CTRL_ENABLE | CTRL_STEP_ID_TAG);

	return 0 ;
}


/* ----------------------------------------------------------------------------------------------- */
/* ADCTSC init
 *
 * Handle mmap for ADCTSC , and initial ADCTSC .
 *
 *	@return : 0 for error , 1 for success .
 *
 *	@Note :  iolib_init() will run this function automatically
 */
int BBBIO_ADCTSC_Init()
{
	unsigned int *reg = NULL;
	unsigned int FIFO_count = 0;
	unsigned int FIFO_data = 0;
	int i ;

	if (memh == 0) {
#ifdef BBBIO_LIB_DBG
		fprintf(stderr, "BBBIO_ADCTSC_Init : memory not mapped?\n");
#endif
		return 0;
	}

	adctsc_ptr = mmap(0, ADCTSC_MMAP_LEN, PROT_READ | PROT_WRITE, MAP_SHARED, memh, ADCTSC_MMAP_ADDR);
	if(adctsc_ptr == MAP_FAILED) {
#ifdef BBBIO_LIB_DBG
		fprintf(stderr, "BBBIO_ADCTSC_Init: ADCTSC mmap failure!\n");
#endif
		return 0;
	}

	/* Enable module Clock  */
	reg = (void *)cm_wkup_addr + BBBIO_CM_WKUP_ADC_TSC_CLKCTRL;
	*reg = 0x2 ;

	/* Pre-disable module work */
	reg = (void *)adctsc_ptr + ADCTSC_CTRL;
	*reg &= ~0x1 ;

	/* Default ADC module configure*/
//	BBBIO_ADCTSC_module_ctrl(35, ADCRANGE_MIN_RANGE, ADCRANGE_MAX_RANGE);	/* 44100 hz */
	BBBIO_ADCTSC_module_ctrl(BBBIO_ADC_WORK_MODE_BUSY_POLLING, 1);
	BBBIO_ADCTSC_set_range(ADCRANGE_MIN_RANGE, ADCRANGE_MAX_RANGE);

        /* Default channel configure */
	BBBIO_ADCTSC_channel_ctrl(BBBIO_ADC_AIN0, BBBIO_ADC_STEP_MODE_SW_ONE_SHOOT, 0, 1, BBBIO_ADC_STEP_AVG_1, NULL, 0);
	BBBIO_ADCTSC_channel_ctrl(BBBIO_ADC_AIN1, BBBIO_ADC_STEP_MODE_SW_ONE_SHOOT, 0, 1, BBBIO_ADC_STEP_AVG_1, NULL, 0);
	BBBIO_ADCTSC_channel_ctrl(BBBIO_ADC_AIN2, BBBIO_ADC_STEP_MODE_SW_ONE_SHOOT, 0, 1, BBBIO_ADC_STEP_AVG_1, NULL, 0);
	BBBIO_ADCTSC_channel_ctrl(BBBIO_ADC_AIN3, BBBIO_ADC_STEP_MODE_SW_ONE_SHOOT, 0, 1, BBBIO_ADC_STEP_AVG_1, NULL, 0);
	BBBIO_ADCTSC_channel_ctrl(BBBIO_ADC_AIN4, BBBIO_ADC_STEP_MODE_SW_ONE_SHOOT, 0, 1, BBBIO_ADC_STEP_AVG_1, NULL, 0);
	BBBIO_ADCTSC_channel_ctrl(BBBIO_ADC_AIN5, BBBIO_ADC_STEP_MODE_SW_ONE_SHOOT, 0, 1, BBBIO_ADC_STEP_AVG_1, NULL, 0);
	BBBIO_ADCTSC_channel_ctrl(BBBIO_ADC_AIN6, BBBIO_ADC_STEP_MODE_SW_ONE_SHOOT, 0, 1, BBBIO_ADC_STEP_AVG_1, NULL, 0);

	/* Clear FIFO  */
	FIFO_count = *((unsigned int*)((void *)adctsc_ptr + ADCTSC_FIFO0COUNT));
	for(i = 0 ; i < FIFO_count ; i++) {
		FIFO_data = *((unsigned int*)((void *)adctsc_ptr + ADCTSC_FIFO0DATA));
	}

	FIFO_count = *((unsigned int*)((void *)adctsc_ptr + ADCTSC_FIFO1COUNT));
        for(i = 0 ; i < FIFO_count ; i++) {
		FIFO_data = *((unsigned int*)((void *)adctsc_ptr + ADCTSC_FIFO1DATA));
        }

	/* init work struct */
	ADCTSC.FIFO[0].reg_count = (void *)adctsc_ptr + ADCTSC_FIFO0COUNT;
	ADCTSC.FIFO[0].reg_data = (void *)adctsc_ptr + ADCTSC_FIFO0DATA;
	ADCTSC.FIFO[0].next = &ADCTSC.FIFO[1];
	ADCTSC.FIFO[1].reg_count = (void *)adctsc_ptr + ADCTSC_FIFO1COUNT;
	ADCTSC.FIFO[1].reg_data = (void *)adctsc_ptr + ADCTSC_FIFO1DATA;
	ADCTSC.FIFO[1].next = &ADCTSC.FIFO[0];
	ADCTSC.channel_en = 0;

	/* init work mode ad busy_polling mode */
	ADCTSC.work_mode = BBBIO_ADC_WORK_MODE_BUSY_POLLING;
	return 1;
}

