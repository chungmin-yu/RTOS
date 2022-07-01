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
#define  N_TASKS                        3        /* Number of identical tasks                          */

/*
*********************************************************************************************************
*                                               VARIABLES
*********************************************************************************************************
*/
OS_EVENT *R1, *R2;
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


/*
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************
*/

        void  PrintCtxSwMessage(void);
        void  Task1(void *data);                      /* Function prototypes of tasks                  */
        void  Task2(void *data);                      /* Function prototypes of tasks                  */
        void  Task3(void *data);                      /* Function prototypes of tasks                  */
        void  TaskStart(void *data);                  /* Function prototypes of Startup task           */
static  void  TaskStartCreateTasks(void);


/*$PAGE*/
/*
*********************************************************************************************************
*                                                MAIN
*********************************************************************************************************
*/

void  main (void)
{
    //PC_DispClrScr(DISP_FGND_WHITE + DISP_BGND_BLACK);      /* Clear the screen                         */

    INT8U err1, err2;

    OSInit();                                              /* Initialize uC/OS-II                      */

    PC_DOSSaveReturn();                                    /* Save environment to return to DOS        */
    PC_VectSet(uCOS, OSCtxSw);                             /* Install uC/OS-II's context switch vector */

    R1 = OSMutexCreate(1,&err1);
    R2 = OSMutexCreate(2,&err2);
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

    OS_ENTER_CRITICAL();
    PC_VectSet(0x08, OSTickISR);                           /* Install uC/OS-II's clock tick ISR        */
    PC_SetTickRate(OS_TICKS_PER_SEC);                      /* Reprogram tick rate                      */
    OS_EXIT_CRITICAL();

    OSStatInit();                                          /* Initialize uC/OS-II's statistics         */

    TaskStartCreateTasks();                                /* Create all the application tasks         */
	OSTimeSet(0);
    TimeStart = 1;
    for (;;) {
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
*                                             CREATE TASKS
*********************************************************************************************************
*/

static  void  TaskStartCreateTasks (void)
{
    INT8U  i;
    INT8U  err;

    OSTaskCreate(Task1, (void *)&TaskData[0], &TaskStk[0][TASK_STK_SIZE - 1], 3);
    OSTaskCreate(Task2, (void *)&TaskData[1], &TaskStk[1][TASK_STK_SIZE - 1], 4);
    OSTaskCreate(Task3, (void *)&TaskData[2], &TaskStk[2][TASK_STK_SIZE - 1], 5);

}

/*
*********************************************************************************************************
*                                                  TASKS
*********************************************************************************************************
*/


/*void Task (void *pdata)
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
            printf("time:%d task:%d exceed deadline\n", end, OSTCBCur->OSTCBPrio);
        }
        toDelay = (OSTCBCur->period) - (end-start);
        start = start + (OSTCBCur->period);
        
        OS_ENTER_CRITICAL();
        OSTCBCur->compTime=c;                     // reset the counter (c ticks for computation)
        OS_EXIT_CRITICAL();
        OSTimeDly (toDelay);                      // delay and wait (P-C) times
    }
}*/

void PrintCtxSwMessage(void){
    static int i;
    for(; i < CtxSwMessageTop; i++){
        printf("%s", CtxSwMessage[i]);
    }
}


// testcase 1

void Task1(void *pdata)
{
    INT8U err;
    int start = 8;
    int startDelayTime;
    startDelayTime = start - OSTime;
    if(startDelayTime > 0) 
        OSTimeDly(startDelayTime);

    OSTCBCur->compTime = 2;
    while(OSTCBCur->compTime > 0) {}

    // Lock R1
    OSTCBCur->compTime = 2;
    OSMutexPend(R1, 0, &err);
    while(OSTCBCur->compTime > 0) {}

    // Lock R2
    OSTCBCur->compTime = 2;
    OSMutexPend(R2, 0, &err);
    while(OSTCBCur->compTime > 0) {}

    OSMutexPost(R2);
    OSMutexPost(R1);

    OSTaskDel(OS_PRIO_SELF);
}
void Task2(void *pdata)
{
    INT8U err;
    int start = 4;
    int startDelayTime;
    startDelayTime = start - OSTime;
    if(startDelayTime > 0) 
        OSTimeDly(startDelayTime);

    OSTCBCur->compTime = 2;
    while(OSTCBCur->compTime > 0) {}

    // Lock R2
    OSTCBCur->compTime = 4;
    OSMutexPend(R2, 0, &err);
    while(OSTCBCur->compTime > 0) {}

    OSMutexPost(R2);

    OSTaskDel(OS_PRIO_SELF);
}
void Task3(void *pdata)
{
    INT8U err;
    int start = 0;
    int startDelayTime;
    startDelayTime = start - OSTime;
    if(startDelayTime > 0) 
        OSTimeDly(startDelayTime);

    OSTCBCur->compTime = 2;
    while(OSTCBCur->compTime > 0) {}

    // Lock R1
    OSTCBCur->compTime = 7;
    OSMutexPend(R1, 0, &err);
    while(OSTCBCur->compTime > 0) {}

    OSMutexPost(R1);

    OSTaskDel(OS_PRIO_SELF);
}



// testcase 2
/*
void Task1(void *pdata)
{
    INT8U err;
    int start = 5;
    int startDelayTime;
    startDelayTime = start - OSTime;
    if(startDelayTime > 0) 
        OSTimeDly(startDelayTime);

    OSTCBCur->compTime = 2;
    while(OSTCBCur->compTime > 0) {}

    // Lock R2
    OSTCBCur->compTime = 3;
    OSMutexPend(R2, 0, &err);
    while(OSTCBCur->compTime > 0) {}

    // Lock R1
    OSTCBCur->compTime = 3;
    OSMutexPend(R1, 0, &err);
    while(OSTCBCur->compTime > 0) {}
    OSMutexPost(R1);

    OSTCBCur->compTime = 3;
    while(OSTCBCur->compTime > 0) {}
    OSMutexPost(R2);

    OSTaskDel(OS_PRIO_SELF);
}
void Task2(void *pdata)
{
    INT8U err;
    int start = 0;
    int startDelayTime;
    startDelayTime = start - OSTime;
    if(startDelayTime > 0) 
        OSTimeDly(startDelayTime);

    OSTCBCur->compTime = 2;
    while(OSTCBCur->compTime > 0) {}

    // Lock R1
    OSTCBCur->compTime = 6;
    OSMutexPend(R1, 0, &err);
    while(OSTCBCur->compTime > 0) {}

    // Lock R2
    OSTCBCur->compTime = 2;
    OSMutexPend(R2, 0, &err);
    while(OSTCBCur->compTime > 0) {}
    OSMutexPost(R2);

    OSTCBCur->compTime = 2;
    while(OSTCBCur->compTime > 0) {}
    OSMutexPost(R1);

    OSTaskDel(OS_PRIO_SELF);
}*/

