#ifndef BBBIO_ADCTSC_H
#define BBBIO_ADCTSC_H
//----------------------------------------------------------------------------
int BBBIO_ADCTSC_Init();
void BBBIO_ADCTSC_step_work();

unsigned int BBBIO_ADCTSC_work(unsigned int fetch_size);
void BBBIO_ADCTSC_channel_ctrl(unsigned int chn_ID, int mode, int sample_avg, unsigned int *buf, unsigned int buf_size);
void BBBIO_ADCTSC_module_ctrl(unsigned int clkdiv, int L_range, int H_range);

#define BBBIO_ADCTSC_channel_enable(A) BBBIO_ADCTSC_channel_status(A,1)
#define BBBIO_ADCTSC_channel_disable(A) BBBIO_ADCTSC_channel_status(A,0)

void BBBIO_ADCTSC_channel_status(int chn_ID ,int enable);
/* ------------------------------------------------------------------------- */
#endif
