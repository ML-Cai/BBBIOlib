#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <math.h>
#include <signal.h>
#define __USE_GNU
#include <sched.h>
#include "../../BBBio_lib/BBBiolib.h"
//-----------------------------------------------------------------
void keypad_4x4_scan();
void Thread_4x4_scan();

// S03T 2BBMG	0.33s /60 degree
//float SM_speed = 0.33f / 60.0f ;	// rotate speed per degree (5.5ms)
float SM_speed = 0.30f / 60.0f ;      // rotate speed per degree (5.5ms)

const float SM_Cycle = 0.02 ;		//50HZ

float Angle_Arm_1 =-1 ;
float Angle_Arm_2 =-1 ;
float Angle_Arm_3 =-1 ;
float Angle_Hand =-1;
float Angle_Buttom =-1 ;

float Angle_Arm_1_t =-1 ;
float Angle_Arm_2_t =-1 ;
float Angle_Arm_3_t =-1 ;
float Angle_Hand_t =-1;
float Angle_Buttom_t =-1 ;


#define SM_DEVICE_COUNT	5
#define SM_BUTTOM	0
#define SM_ARM1		1
#define SM_ARM2		2
#define	SM_ARM3		3
#define SM_HAND		4

const unsigned int key_map[4][4]={{'0','1','2','3'},
                                  {'4','5','6','7'},
                                  {'8','9','A','B'},
                                  {'C','D','E','F'}};

unsigned int key_status[4][4]={0};

/*
int Action[10][5]={{110 ,100,70 ,110 ,150} ,

                   {105 ,150,70 ,110 ,110 } ,
                   {105 ,150,75 ,40 ,110 } ,
                   {105 ,100,75 ,40 ,110 } ,

                   {105 ,100,100 ,40 ,90 } ,
                   {130 ,100,100 ,40 ,90 } ,

                   {130 ,100,65 ,30 ,65 } ,
                   {130 ,150,65 ,30 ,65 } ,
                   {130 ,150,60 ,110 ,65 } ,

				   {110 ,100,70 ,110 ,150 } };
*/

int Action[12][5]={{100 ,150,70 ,110 ,150} ,

                   {100 ,150 ,65 ,20 ,70 } ,
                   {100 ,90 ,65 ,20 ,70 } ,
                   {100 ,90 ,65 ,20 ,70 } ,
		   {100 ,90 ,65 ,20 ,70 } ,
                   {100 ,90 ,100 ,20 ,70 } ,

                   {135 ,90 ,100 ,20 ,70} ,
                   {135 ,90 ,70 ,20 ,70 } ,
                   {135 ,90 ,70 ,20 ,70 } ,

                   {135 ,150 ,70 ,20 ,70 } ,
                   {135 ,150 ,100 ,20 ,70 } ,

                   {135 ,100 ,100 ,110 ,150 } };
//-----------------------------------------------------------------
int GetPulseCycleCount(int angle)
{
	if (angle <0 || angle >204)	// S03T only have 180 angle
	    return -1;

	return (int)(((float)angle *SM_speed)/SM_Cycle +1) ;
}
//-----------------------------------------------------------------
// 700 ~ 2200
int GetPulseCycleDelay(int angle)
{
	if (angle <0 || angle >204)     // S03T only have 180 angle
            return -1;

	// resolution 12
	// -3 ~ 14 * 100us (800us ~ 2400us)
	return (int)((float)angle * 4.0f /45.0f +0.5f) -2;	// move angle = 2400 - 800 = 1600
								// = 1600ns / 180
								// = 16 seg / 180 , per seg 100us .
								// = 4 / 45
}
//-----------------------------------------------------------------
int
main(void)
{
	iolib_init();
	BBBIO_sys_Enable_GPIO(BBBIO_GPIO2);

	BBBIO_GPIO_set_dir(BBBIO_GPIO2 ,	// key pad pin
                           BBBIO_GPIO_PIN_12 |BBBIO_GPIO_PIN_10 |BBBIO_GPIO_PIN_8 |BBBIO_GPIO_PIN_6 ,   // input
                           BBBIO_GPIO_PIN_13 |BBBIO_GPIO_PIN_11 |BBBIO_GPIO_PIN_9 |BBBIO_GPIO_PIN_7 );  // output
	int i , j ;
	float Buttom_duty =0;
	float Hand_duty =0;
	float Arm_1_duty =0;
	float Arm_2_duty =0;
	float Arm_3_duty =0;

	// recover
        BBBIO_PWMSS_Disable(0);
        BBBIO_PWMSS_Disable(1);
        BBBIO_PWMSS_Disable(2);
	Buttom_duty = 4.0f + 90.0f* 0.04444f ;
	Hand_duty = 4.0f + 90.0f* 0.04444f ;
	Arm_1_duty = 4.0f + 90.0f* 0.04444f ;
	Arm_2_duty = 4.0f + 110.0f* 0.04444f ;
	Arm_3_duty = 4.0f + 110.0f* 0.04444f ;
	BBBIO_PWMSS_Setting(0 , 50.0f ,Buttom_duty , Hand_duty);
	BBBIO_PWMSS_Setting(1 , 50.0f ,Arm_1_duty , Arm_1_duty);
	BBBIO_PWMSS_Setting(2 , 50.0f ,Arm_2_duty , Arm_3_duty);
	BBBIO_PWMSS_Enable(0);
	BBBIO_PWMSS_Enable(1);
	BBBIO_PWMSS_Enable(2);
	Angle_Arm_1 =90 ;
	Angle_Arm_2 =110 ;
	Angle_Arm_3 =150;
	Angle_Hand =90 ;
	Angle_Buttom = 90 ;

        Angle_Arm_1_t =90 ;
        Angle_Arm_2_t =110 ;
        Angle_Arm_3_t =150;
        Angle_Hand_t =90 ;
        Angle_Buttom_t = 90 ;

	sleep(1);
/*
        // create 4x4 scan therad
        pthread_t pid_4x4 ;
        int ret ;
        ret = pthread_create(&pid_4x4, NULL, (void *)Thread_4x4_scan, NULL);
	sleep(1);

        //-----------------------------------------
        while(key_status[3][3]==1)      //f
        {
		if( fabs(Angle_Buttom -Angle_Buttom_t) >0.01f || fabs(Angle_Hand -Angle_Hand_t) >0.01f )
		{
		    Buttom_duty = 4.0f + Angle_Buttom* 0.04444f ;
                    Hand_duty = 4.0f + Angle_Hand* 0.04444f ;
		    BBBIO_PWMSS_Setting(0 , 50.0f ,Buttom_duty , Hand_duty);
//		    BBBIO_PWMSS_Enable(0);
		    Angle_Hand_t =Angle_Hand ;
                    Angle_Buttom_t = Angle_Buttom ;
		}

		if(fabs(Angle_Arm_1 -Angle_Arm_1_t) >0.01f)
		{
		    Arm_1_duty = 4.0f + Angle_Arm_1* 0.04444f ;
		    BBBIO_PWMSS_Setting(1 , 50.0f ,Arm_1_duty , Arm_1_duty);
//		    BBBIO_PWMSS_Enable(1);
		    Angle_Arm_1_t =Angle_Arm_1 ;
		}

		if(fabs(Angle_Arm_2 -Angle_Arm_2_t) >0.01f || fabs(Angle_Arm_3 -Angle_Arm_3_t) >0.01f)
		{
		    Arm_2_duty = 4.0f + Angle_Arm_2* 0.04444f ;
		    Arm_3_duty = 4.0f + Angle_Arm_3* 0.04444f ;
		    BBBIO_PWMSS_Setting(2 , 50.0f ,Arm_2_duty , Arm_3_duty);
//		    BBBIO_PWMSS_Enable(2);
		    Angle_Arm_2_t =Angle_Arm_2 ;
		    Angle_Arm_3_t =Angle_Arm_3;
		}
		usleep(550000);

	}
*/


for(j=0 ;j<2 ; j++)
{
	for(i=0; i<12 ; i++)
	{
		Buttom_duty = 4.0f + Action[i][0] * 0.04444f ;
	        Hand_duty = 4.0f + Action[i][1] * 0.04444f ;
		Arm_1_duty = 4.0f + Action[i][2]  * 0.04444f ;
	        Arm_2_duty = 4.0f + Action[i][3] * 0.04444f ;
        	Arm_3_duty = 4.0f + Action[i][4] * 0.04444f ;
	        BBBIO_PWMSS_Setting(0 , 50.0f ,Buttom_duty , Hand_duty);
	        BBBIO_PWMSS_Setting(1 , 50.0f ,Arm_1_duty , Arm_1_duty);
	        BBBIO_PWMSS_Setting(2 , 50.0f ,Arm_2_duty , Arm_3_duty);
		//sleep(1);
		usleep(550000);
	}
	sleep(1);
}

	Buttom_duty = 4.0f + 90* 0.04444f ;
        Hand_duty = 4.0f + 90* 0.04444f ;
        Arm_1_duty = 4.0f + 85* 0.04444f ;
        Arm_2_duty = 4.0f + 110* 0.04444f ;
        Arm_3_duty = 4.0f + 140* 0.04444f ;
        BBBIO_PWMSS_Setting(0 , 50.0f ,Buttom_duty , Hand_duty);
        BBBIO_PWMSS_Setting(1 , 50.0f ,Arm_1_duty , Arm_1_duty);
        BBBIO_PWMSS_Setting(2 , 50.0f ,Arm_2_duty , Arm_3_duty);
	sleep(2);

        BBBIO_PWMSS_Disable(0);
        BBBIO_PWMSS_Disable(1);
        BBBIO_PWMSS_Disable(2);
	//---------------------------
	 printf("Waiting F\n");
//	pthread_join(pid_4x4,NULL);
	printf("Finish\n");
	iolib_free();
	return(0);
}


//------------------------------------------------------------------
void keypad_4x4_scan()
{
        int value=0;
        BBBIO_GPIO_low(BBBIO_GPIO2 ,BBBIO_GPIO_PIN_13);     //line 1
        value = BBBIO_GPIO_get(BBBIO_GPIO2 ,BBBIO_GPIO_PIN_12 |BBBIO_GPIO_PIN_10 |BBBIO_GPIO_PIN_8 |BBBIO_GPIO_PIN_6 );
        BBBIO_GPIO_high(BBBIO_GPIO2 ,BBBIO_GPIO_PIN_13);
        key_status[0][0] = value >> 6 & 0x01;
        key_status[0][1] = value >> 8 & 0x01;
        key_status[0][2] = value >> 10 & 0x01;
        key_status[0][3] = value >> 12 & 0x01;

        BBBIO_GPIO_low(BBBIO_GPIO2 ,BBBIO_GPIO_PIN_11);     //line 2
        value = BBBIO_GPIO_get(BBBIO_GPIO2 ,BBBIO_GPIO_PIN_12 |BBBIO_GPIO_PIN_10 |BBBIO_GPIO_PIN_8 |BBBIO_GPIO_PIN_6 );
        BBBIO_GPIO_high(BBBIO_GPIO2 ,BBBIO_GPIO_PIN_11);
        key_status[1][0] = value >> 6 & 0x01;
        key_status[1][1] = value >> 8 & 0x01;
        key_status[1][2] = value >> 10 & 0x01;
        key_status[1][3] = value >> 12 & 0x01;

        BBBIO_GPIO_low(BBBIO_GPIO2 ,BBBIO_GPIO_PIN_9);     //line 3
        value = BBBIO_GPIO_get(BBBIO_GPIO2 ,BBBIO_GPIO_PIN_12 |BBBIO_GPIO_PIN_10 |BBBIO_GPIO_PIN_8 |BBBIO_GPIO_PIN_6 );
        BBBIO_GPIO_high(BBBIO_GPIO2 ,BBBIO_GPIO_PIN_9);
        key_status[2][0] = value >> 6 & 0x01;
        key_status[2][1] = value >> 8 & 0x01;
        key_status[2][2] = value >> 10 & 0x01;
        key_status[2][3] = value >> 12 & 0x01;

        BBBIO_GPIO_low(BBBIO_GPIO2 ,BBBIO_GPIO_PIN_7);     //line 4
        value = BBBIO_GPIO_get(BBBIO_GPIO2 ,BBBIO_GPIO_PIN_12 |BBBIO_GPIO_PIN_10 |BBBIO_GPIO_PIN_8 |BBBIO_GPIO_PIN_6 );
        BBBIO_GPIO_high(BBBIO_GPIO2 ,BBBIO_GPIO_PIN_7);
        key_status[3][0] = value >> 6 & 0x01;
        key_status[3][1] = value >> 8 & 0x01;
        key_status[3][2] = value >> 10 & 0x01;
        key_status[3][3] = value >> 12 & 0x01;
}
//--------------------------------------------------------------------
void Thread_4x4_scan()
{
	int i, j ;
	while(1)
	{

	    keypad_4x4_scan();
            if(key_status[3][3]==0)     //f
                break ;

	    if(key_status[2][0]==0) Angle_Hand =110.0f ;  	//8
	    if(key_status[3][0]==0) Angle_Hand =150.0f ; 	//C

            if(key_status[0][0]==0) Angle_Buttom +=5.0f ;  //0
            if(key_status[1][0]==0) Angle_Buttom -=5.0f ;  //4
            if(Angle_Buttom < 20) Angle_Buttom =20.0f ;
            if(Angle_Buttom > 160) Angle_Buttom =160.0f ;

            if(key_status[0][1]==0) Angle_Arm_1 +=5.0f ;   //1
            if(key_status[1][1]==0) Angle_Arm_1 -=5.0f ;   //2
            if(Angle_Arm_1 < 20) Angle_Arm_1 =20.0f ;
            if(Angle_Arm_1 > 160) Angle_Arm_1 =160.0f ;

            if(key_status[0][2]==0) Angle_Arm_2 +=5.0f ;   //2
            if(key_status[1][2]==0) Angle_Arm_2 -=5.0f ;   //6
            if(Angle_Arm_2 < 20) Angle_Arm_2 =20.0f ;
            if(Angle_Arm_2 > 160) Angle_Arm_2 =160.0f ;

            if(key_status[0][3]==0) Angle_Arm_3 +=5.0f ;   //3
            if(key_status[1][3]==0) Angle_Arm_3 -=5.0f ;   //7
            if(Angle_Arm_3 < 20) Angle_Arm_3 =20.0f ;
            if(Angle_Arm_3 > 160) Angle_Arm_3 =160.0f ;


	    printf("\rButtom : %f \tArm1 : %f\t Arm2 : %f\tArm3 : %f\t Hand : %f",Angle_Buttom,Angle_Arm_1 ,Angle_Arm_2 ,Angle_Arm_3 ,Angle_Hand);

	    usleep(100000);
	}
	pthread_exit(NULL);
}
