/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*
*                           (c) Copyright 1992-2002, Jean J. Labrosse, Weston, FL
*                                           All Rights Reserved
*
*                                               EXAMPLE #1
*********************************************************************************************************
*/

#include "includes.h"

/*
*********************************************************************************************************
*                                               CONSTANTS
*********************************************************************************************************
*/

#define  TASK_STK_SIZE                 512       /* Size of each task's stacks (# of WORDs)            */
#define  N_TASKS                        2        /* Number of identical tasks                          */

/*
*********************************************************************************************************
*                                               VARIABLES
*********************************************************************************************************
*/
struct period {
    int exeTime;
    int period;
};

char CtxSwMessage[CtxSwMessageSize][50];
int CtxSwMessageTop = 0;
int TimeStart = 0;
OS_EVENT     *PrintCtxSwMbox;                         /* Message box for tasks                         */

OS_STK        TaskStk[N_TASKS][TASK_STK_SIZE];        /* Tasks stacks                                  */
OS_STK        TaskStartStk[TASK_STK_SIZE];

struct period   TaskData[N_TASKS];                      /* Parameters to pass to each task               */
//char          TaskData[N_TASKS];                      /* Parameters to pass to each task               */
//OS_EVENT     *RandomSem;

/*
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************
*/

        void  PrintCtxSwMessage();
        void  Task(void *data);                       /* Function prototypes of tasks                  */
        void  TaskStart(void *data);                  /* Function prototypes of Startup task           */
static  void  TaskStartCreateTasks(void);
static  void  TaskStartDispInit(void);
static  void  TaskStartDisp(void);

/*$PAGE*/
/*
*********************************************************************************************************
*                                                MAIN
*********************************************************************************************************
*/

void  main (void)
{
    PC_DispClrScr(DISP_FGND_WHITE + DISP_BGND_BLACK);      /* Clear the screen                         */

    OSInit();                                              /* Initialize uC/OS-II                      */

    PC_DOSSaveReturn();                                    /* Save environment to return to DOS        */
    PC_VectSet(uCOS, OSCtxSw);                             /* Install uC/OS-II's context switch vector */

    //RandomSem   = OSSemCreate(1);                          /* Random number semaphore                  */

    OSTaskCreate(TaskStart, (void *)0, &TaskStartStk[TASK_STK_SIZE - 1], 0);
    OSStart();                                             /* Start multitasking                       */
}


/*
*********************************************************************************************************
*                                              STARTUP TASK
*********************************************************************************************************
*/
void  TaskStart (void *pdata)
{
#if OS_CRITICAL_METHOD == 3                                /* Allocate storage for CPU status register */
    OS_CPU_SR  cpu_sr;
#endif
    char       s[100];
    INT16S     key;


    pdata = pdata;                                         /* Prevent compiler warning                 */

    //TaskStartDispInit();                                   /* Initialize the display                   */

    OS_ENTER_CRITICAL();
    PC_VectSet(0x08, OSTickISR);                           /* Install uC/OS-II's clock tick ISR        */
    PC_SetTickRate(OS_TICKS_PER_SEC);                      /* Reprogram tick rate                      */
    OS_EXIT_CRITICAL();

    OSStatInit();                                          /* Initialize uC/OS-II's statistics         */

    TaskStartCreateTasks();                                /* Create all the application tasks         */
	OSTimeSet(0);
	TimeStart = 1;
    for (;;) {
        //TaskStartDisp();                                  /* Update the display                       */
        PrintCtxSwMessage();

        if (PC_GetKey(&key) == TRUE) {                     /* See if key has been pressed              */
            if (key == 0x1B) {                             /* Yes, see if it's the ESCAPE key          */
                PC_DOSReturn();                            /* Return to DOS                            */
            }
        }

        OSCtxSwCtr = 0;                                    /* Clear context switch counter             */
        OSTimeDlyHMSM(0, 0, 1, 0);                         /* Wait one second                          */
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                        INITIALIZE THE DISPLAY
*********************************************************************************************************
*/

static  void  TaskStartDispInit (void)
{
/*                                1111111111222222222233333333334444444444555555555566666666667777777777 */
/*                      01234567890123456789012345678901234567890123456789012345678901234567890123456789 */
    PC_DispStr( 0,  0, "                         uC/OS-II, The Real-Time Kernel                         ", DISP_FGND_WHITE + DISP_BGND_RED + DISP_BLINK);
    PC_DispStr( 0,  1, "                                Jean J. Labrosse                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0,  2, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0,  3, "                                    EXAMPLE #1                                  ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0,  4, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0,  5, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0,  6, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0,  7, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0,  8, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0,  9, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 10, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 11, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 12, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 13, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 14, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 15, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 16, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 17, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 18, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 19, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 20, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 21, "                                                                                ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 22, "#Tasks          :        CPU Usage:     %                                       ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 23, "#Task switch/sec:                                                               ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
    PC_DispStr( 0, 24, "                            <-PRESS 'ESC' TO QUIT->                             ", DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY + DISP_BLINK);
/*                                1111111111222222222233333333334444444444555555555566666666667777777777 */
/*                      01234567890123456789012345678901234567890123456789012345678901234567890123456789 */
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                           UPDATE THE DISPLAY
*********************************************************************************************************
*/

static  void  TaskStartDisp (void)
{
    char   s[80];


    sprintf(s, "%5d", OSTaskCtr);                                  /* Display #tasks running               */
    PC_DispStr(18, 22, s, DISP_FGND_YELLOW + DISP_BGND_BLUE);

#if OS_TASK_STAT_EN > 0
    sprintf(s, "%3d", OSCPUUsage);                                 /* Display CPU usage in %               */
    PC_DispStr(36, 22, s, DISP_FGND_YELLOW + DISP_BGND_BLUE);
#endif

    sprintf(s, "%5d", OSCtxSwCtr);                                 /* Display #context switches per second */
    PC_DispStr(18, 23, s, DISP_FGND_YELLOW + DISP_BGND_BLUE);

    sprintf(s, "V%1d.%02d", OSVersion() / 100, OSVersion() % 100); /* Display uC/OS-II's version number    */
    PC_DispStr(75, 24, s, DISP_FGND_YELLOW + DISP_BGND_BLUE);

    switch (_8087) {                                               /* Display whether FPU present          */
        case 0:
             PC_DispStr(71, 22, " NO  FPU ", DISP_FGND_YELLOW + DISP_BGND_BLUE);
             break;

        case 1:
             PC_DispStr(71, 22, " 8087 FPU", DISP_FGND_YELLOW + DISP_BGND_BLUE);
             break;

        case 2:
             PC_DispStr(71, 22, "80287 FPU", DISP_FGND_YELLOW + DISP_BGND_BLUE);
             break;

        case 3:
             PC_DispStr(71, 22, "80387 FPU", DISP_FGND_YELLOW + DISP_BGND_BLUE);
             break;
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                             CREATE TASKS
*********************************************************************************************************
*/

static  void  TaskStartCreateTasks (void)
{
    INT8U  i;
    INT8U  err;

    TaskData[0].exeTime = 1;
    TaskData[0].period = 3;
    TaskData[1].exeTime = 3;
    TaskData[1].period = 6;
    //TaskData[2].exeTime = 4;
    //TaskData[2].period = 9;

    //for (i = 0; i < N_TASKS; i++) {                        /* Create N_TASKS identical tasks           */
    //    TaskData[i] = '0' + i;                             /* Each task will display its own letter    */
    //    OSTaskCreate(Task, (void *)&TaskData[i], &TaskStk[i][TASK_STK_SIZE - 1], i + 1);
    //}

    for (i = 0; i < N_TASKS; i++) {
        err = OSTaskCreate(Task, (void *)&TaskData[i], &TaskStk[i][TASK_STK_SIZE - 1], i + 1);
        if(err!=OS_NO_ERR){
            printf("create task %d errorno %d \n", i, err);
        }
    }
}

/*
*********************************************************************************************************
*                                                  TASKS
*********************************************************************************************************
*/

//void  Task (void *pdata)
//{
//    INT8U  x;
//    INT8U  y;
//    INT8U  err;


//    for (;;) {
//        OSSemPend(RandomSem, 0, &err);           /* Acquire semaphore to perform random numbers        */
//        x = random(80);                          /* Find X position where task number will appear      */
//        y = random(16);                          /* Find Y position where task number will appear      */
//        OSSemPost(RandomSem);                    /* Release semaphore                                  */
                                                 /* Display the task number on the screen              */
//        PC_DispChar(x, y + 5, *(char *)pdata, DISP_FGND_BLACK + DISP_BGND_LIGHT_GRAY);
//        OSTimeDly(1);                            /* Delay 1 clock tick                                 */
//    }
//}

void Task (void *pdata)
{
    int start;   // start time
    int end;     // end time
    int toDelay; 
    struct period *tmpPdata = (struct period*) pdata;
    int c = tmpPdata->exeTime;

    OS_ENTER_CRITICAL();
    OSTCBCur->compTime=c;                         // set the counter (c ticks for computation)
    OSTCBCur->period=tmpPdata->period;               // set the period
    OS_EXIT_CRITICAL();

    start = 0;
    while(1)
    {
        while(OSTCBCur->compTime > 0)             // C ticks
        {
            // do nothing
        }
        end = OSTimeGet();
        if (end > start+OSTCBCur->period){
            printf("time:%d task%d exceed deadline\n", start+OSTCBCur->period, OSTCBCur->OSTCBPrio);
        }
        toDelay = (OSTCBCur->period) - (end-start);
        start = start + (OSTCBCur->period);
        
        OS_ENTER_CRITICAL();
        OSTCBCur->compTime=c;                     // reset the counter (c ticks for computation)
        OS_EXIT_CRITICAL();
        OSTimeDly (toDelay);                      // delay and wait (P-C) times
    }
}

void PrintCtxSwMessage(){
    static int i;
    for(; i < CtxSwMessageTop; i++){
        printf("%s", CtxSwMessage[i]);
    }
}
