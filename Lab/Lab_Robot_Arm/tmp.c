#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
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


#define SM_DEVICE_COUNT	5
#define SM_BUTTOM	0
#define SM_ARM1		1
#define SM_ARM2		2
#define	SM_ARM3		3
#define SM_HAND		4

int PulseQueue[SM_DEVICE_COUNT][10000] ={0};	// 1s
int PulseIndex =0;
int PulseFlag =0;

const unsigned int key_map[4][4]={{'0','4','8','C'},
                                  {'1','5','9','D'},
                                  {'2','6','A','E'},
                                  {'3','7','B','F'}};

unsigned int key_status[4][4]={0};

float Action[7][5]={{90.0f ,90.0f ,110.0f ,150.0f ,168.75f} ,
		    {90.0f ,63.75f ,42.5f ,97.5f ,168.75f} ,
		    {90.0f ,63.75f ,42.5f ,97.5f ,101.25f} ,
		    {90.0f ,86.25f ,42.5f ,108.75f ,101.25f} ,
                    {30.0f ,86.25f ,42.5f ,108.75f ,101.25f} ,
                    {30.0f ,86.25f ,42.5f ,108.75f ,168.75f} ,
		    {90.0f ,90.0f ,110.0f ,150.0f ,90.0f} };

//-----------------------------------------------------------------
void ActionPush(unsigned int PartID ,int Angle ,int Reset)
{
	int CycleCount ;
        int CycleDelay ;
	int i , Cyci ;

	if(Reset >0)	//if reset , worst case is rotate 180
	    CycleCount = GetPulseCycleCount(Angle) ;
	else
            CycleCount = GetPulseCycleCount(180) ;

        CycleDelay = GetPulseCycleDelay(Angle) ;

	for(Cyci =0 ;Cyci <CycleCount ; Cyci++) // mensure rotatefinish
	{
	    // high
	    for(i=0 ; i< 10+CycleDelay ; i++)
	    {
	    	PulseQueue[PartID][Cyci*200+i] =1 ;
	    }

	    // low
            for(i=10+CycleDelay ; i< 200; i++)
            {
            	PulseQueue[PartID][Cyci*200+i] =0 ;
            }
	    //PulseIndex +=200 ;
	}
	PulseIndex = 200 *CycleCount;
}
//-----------------------------------------------------------------
void ActionRun()
{
	static int RunCounter=0;
	int LowValue =0;
	int HighValue =0;

	// assian pin value
	if(PulseQueue[SM_BUTTOM][RunCounter]==0)	// p8_11
	    LowValue |=  BBBIO_GPIO_PIN_13 ;
	else
	    HighValue |=  BBBIO_GPIO_PIN_13 ;


        if(PulseQueue[SM_ARM1][RunCounter]==0)				// p8_12 p8_15
            LowValue |=  (BBBIO_GPIO_PIN_12 | BBBIO_GPIO_PIN_15) ;
        else
            HighValue |=  (BBBIO_GPIO_PIN_12 | BBBIO_GPIO_PIN_15 );


	if(PulseQueue[SM_ARM2][RunCounter]==0)	// p8_16
            LowValue |=  BBBIO_GPIO_PIN_14 ;
        else
            HighValue |=  BBBIO_GPIO_PIN_14 ;


	if(PulseQueue[SM_ARM3][RunCounter]==0)
            LowValue |=  BBBIO_GPIO_PIN_5 ;
        else
            HighValue |=  BBBIO_GPIO_PIN_5 ;



        if(PulseQueue[SM_HAND][RunCounter]==0)
            LowValue |=  BBBIO_GPIO_PIN_4 ;
        else
            HighValue |=  BBBIO_GPIO_PIN_4 ;


	// write logic to GPIO
	if(HighValue !=0)
	    BBBIO_GPIO_high(BBBIO_GPIO1 , HighValue);

	if(LowValue !=0)
	    BBBIO_GPIO_low(BBBIO_GPIO1 , LowValue);

	// add run counter
	RunCounter ++ ;
	if(RunCounter ==PulseIndex)
	{
	    RunCounter =0;
	    PulseFlag =0;
	}
}
//-----------------------------------------------------------------
void ActionStart()
{
	// create timer
	void (*old_handler)(int);
        struct itimerval t;
        t.it_interval.tv_usec = 100;	//100 ns interval
        t.it_interval.tv_sec = 0;
        t.it_value.tv_usec = 100;
        t.it_value.tv_sec = 0;

	if( setitimer( ITIMER_REAL, &t, NULL) < 0 )
	{
            printf("setitimer error\n");;
	    return ;
    	}

	PulseFlag =1 ;			// clock flag On
        old_handler = signal( SIGALRM, ActionRun );	//handle signal

	//waiting timer finish
        while(PulseFlag ==1);

	// clear timer
	t.it_interval.tv_usec = 0;
        t.it_interval.tv_sec = 0;
	t.it_value.tv_usec = 0;
        t.it_value.tv_sec = 0;
	if( setitimer( ITIMER_REAL, &t, NULL) < 0 )
        {
            printf("setitimer error\n");;
            return ;
        }

	//reback signal
	signal( SIGALRM, old_handler );

	PulseIndex =0;
}

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
	struct timeval t_start,t_end;
	pid_t PID ;
	struct sched_param param;
	int maxpri = sched_get_priority_max(SCHED_FIFO);

	param.sched_priority =maxpri ;
	PID =getpid();
//	sched_setscheduler(PID , SCHED_FIFO ,&param );

	iolib_init();
	iolib_setdir(8,11, BBBIO_DIR_OUT);	// Server motor
	iolib_setdir(8,12, BBBIO_DIR_OUT);
	iolib_setdir(8,15, BBBIO_DIR_OUT);
	iolib_setdir(8,16, BBBIO_DIR_OUT);
	iolib_setdir(8,22, BBBIO_DIR_OUT);
        iolib_setdir(8,23, BBBIO_DIR_OUT);
	BBBIO_sys_Enable_GPIO(BBBIO_GPIO2);

	BBBIO_GPIO_set_dir(BBBIO_GPIO2 ,	// key pad pin
                           BBBIO_GPIO_PIN_12 |BBBIO_GPIO_PIN_10 |BBBIO_GPIO_PIN_8 |BBBIO_GPIO_PIN_6 ,   // input
                           BBBIO_GPIO_PIN_13 |BBBIO_GPIO_PIN_11 |BBBIO_GPIO_PIN_9 |BBBIO_GPIO_PIN_7 );  // output

	int i , j ;
	// recover
        ActionPush(SM_BUTTOM ,90 ,0);
        ActionPush(SM_ARM1 ,90 ,0);
        ActionPush(SM_ARM2 ,110 ,0);
        ActionPush(SM_ARM3 ,150 ,0);
        ActionPush(SM_HAND ,90 ,0);
        ActionStart();

	Angle_Arm_1 =90 ;
	Angle_Arm_2 =110 ;
	Angle_Arm_3 =150;
	Angle_Hand =90 ;
	Angle_Buttom = 90 ;

        //-----------------------------------------
        // create 4x4 scan therad

        pthread_t pid_4x4 ;
        int ret ;
        ret = pthread_create(&pid_4x4, NULL, (void *)Thread_4x4_scan, NULL);

        //-----------------------------------------

	usleep(100000);

	float max_Angle =0 ;


for(j=0 ;j <5 ; j ++)
{
	for(i=0 ; i<7 ;i++)
	{
            ActionPush(SM_BUTTOM ,Action[i][0] ,0);
            ActionPush(SM_ARM1 ,Action[i][1] ,0);
            ActionPush(SM_ARM2 ,Action[i][2] ,0);
            ActionPush(SM_ARM3 ,Action[i][3] ,0);
            ActionPush(SM_HAND ,Action[i][4] ,0);
            ActionStart();
	}
}

/*
	while(key_status[3][3]==1)	//f
	{

	    ActionPush(SM_BUTTOM ,Angle_Buttom ,0);
            ActionPush(SM_ARM1 ,Angle_Arm_1 ,0);
            ActionPush(SM_ARM2 ,Angle_Arm_2 ,0);
            ActionPush(SM_ARM3 ,Angle_Arm_3 ,0);
            ActionPush(SM_HAND ,Angle_Hand ,0);

            ActionStart();
	}
*/
        ActionPush(SM_BUTTOM ,90 ,0);
        ActionPush(SM_ARM1 ,90 ,0);
        ActionPush(SM_ARM2 ,110 ,0);
        ActionPush(SM_ARM3 ,150 ,0);
        ActionPush(SM_HAND ,90 ,0);
        ActionStart();

	//---------------------------

	pthread_join(pid_4x4,NULL);
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

        struct timeval t_start,t_end;
        pid_t PID ;
        struct sched_param param;
        int maxpri = sched_get_priority_max(SCHED_FIFO)-5;

        param.sched_priority =maxpri ;
        PID =getpid();
//        sched_setscheduler(PID , SCHED_FIFO ,&param );

	int i, j ;
	while(1)
	{

	    keypad_4x4_scan();
            if(key_status[3][3]==0)     //f
                break ;

	    if(key_status[0][2]==0) Angle_Hand =110.0f ;		//8
	    if(key_status[0][3]==0) Angle_Hand =150.0f ; 	//C

            if(key_status[0][0]==0) Angle_Buttom +=11.25f ;  //0
            if(key_status[0][1]==0) Angle_Buttom -=11.25f ;  //3
            if(Angle_Buttom < 40) Angle_Buttom =30.0f ;
            if(Angle_Buttom > 150) Angle_Buttom =150.0f ;

            if(key_status[1][0]==0) Angle_Arm_1 +=11.25f ;   //1
            if(key_status[1][1]==0) Angle_Arm_1 -=11.25f ;   //5
            if(Angle_Arm_1 < 40) Angle_Arm_1 =30.0f ;
            if(Angle_Arm_1 > 150) Angle_Arm_1 =150.0f ;

            if(key_status[2][0]==0) Angle_Arm_2 +=11.25f ;   //2
            if(key_status[2][1]==0) Angle_Arm_2 -=11.25f ;   //6
            if(Angle_Arm_2 < 40) Angle_Arm_2 =30.0f ;
            if(Angle_Arm_2 > 150) Angle_Arm_2 =150.0f ;

            if(key_status[3][0]==0) Angle_Arm_3 +=11.25f ;   //3
            if(key_status[3][1]==0) Angle_Arm_3 -=11.25f ;   //7
            if(Angle_Arm_3 < 40) Angle_Arm_3 =30.0f ;
            if(Angle_Arm_3 > 150) Angle_Arm_3 =150.0f ;


	    printf("\rButtom : %f \tArm1 : %f\t Arm2 : %f\tArm3 : %f\t Hand : %f",Angle_Buttom,Angle_Arm_1 ,Angle_Arm_2 ,Angle_Arm_3 ,Angle_Hand);

	    usleep(200000);
	}
	pthread_exit(NULL);
}
