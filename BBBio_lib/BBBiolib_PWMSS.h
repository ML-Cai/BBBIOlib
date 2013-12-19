#ifndef BBBIO_PWMSS_H
#define BBBIO_PWMSS_H

int BBBIO_PWMSS_Setting(unsigned int PWMID , float HZ ,float dutyA ,float dutyB);
int BBBIO_PWM_Init() ;
int BBBIO_PWMSS_Status(unsigned int PWMID) ;


#endif
