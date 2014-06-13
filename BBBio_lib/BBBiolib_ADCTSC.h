#ifndef BBBIO_ADCTSC_H
#define BBBIO_ADCTSC_H
/*-------------------------------------------------------------------------- */
#define BBBIO_ADC_AIN0	0
#define BBBIO_ADC_AIN1	1
#define BBBIO_ADC_AIN2	2
#define BBBIO_ADC_AIN3	3
#define BBBIO_ADC_AIN4	4
#define BBBIO_ADC_AIN5	5
#define BBBIO_ADC_AIN6	6


/* step config control operator*/
#define BBBIO_ADC_STEP_MODE_SW_ONE_SHOOT	0x0
#define BBBIO_ADC_STEP_MODE_SW_CONTINUOUS	0x1
#define BBBIO_ADC_STEP_MODE_HW_ONE_SHOOT	0x2
#define BBBIO_ADC_STEP_MODE_HW_CONTINUOUS	0x3

#define BBBIO_ADC_STEP_AVG_1	0x0
#define BBBIO_ADC_STEP_AVG_2	0x1
#define BBBIO_ADC_STEP_AVG_4	0x2
#define BBBIO_ADC_STEP_AVG_8	0x3
#define BBBIO_ADC_STEP_AVG_16	0x4

#define BBBIO_ADC_WORK_MODE_BUSY_POLLING	0x1
#define BBBIO_ADC_WORK_MODE_TIMER_INT	0x2

/* ------------------------------------------------------------------------- */
int BBBIO_ADCTSC_Init();
void BBBIO_ADCTSC_step_work();

unsigned int BBBIO_ADCTSC_work(unsigned int fetch_size);
int BBBIO_ADCTSC_channel_buffering(unsigned int chn_ID, unsigned int *buf, unsigned int buf_size);
int BBBIO_ADCTSC_channel_ctrl(unsigned int chn_ID, int mode, int open_dly, int sample_dly, int sample_avg, unsigned int *buf, unsigned int buf_size);
void BBBIO_ADCTSC_module_ctrl(unsigned int work_type, unsigned int clkdiv);
#define BBBIO_ADCTSC_channel_enable(A) BBBIO_ADCTSC_channel_status(A,1)
#define BBBIO_ADCTSC_channel_disable(A) BBBIO_ADCTSC_channel_status(A,0)

int BBBIO_ADCTSC_channel_status(int chn_ID ,int enable);
/* ------------------------------------------------------------------------- */
#endif
