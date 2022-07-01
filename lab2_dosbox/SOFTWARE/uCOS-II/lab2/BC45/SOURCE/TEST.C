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
struct period {
    int exeTime;
    int period;
};

char CtxSwMessage[CtxSwMessageSize][100];
int CtxSwMessageTop = 0;
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
        void  ArgumentSet2(void);
        void  ArgumentSet3(void);
        void  PrintCtxSwMessage(void);
        void  Task(void *data);                       /* Function prototypes of tasks                  */
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
    PC_DispClrScr(DISP_FGND_WHITE + DISP_BGND_BLACK);      /* Clear the screen                         */

    OSInit();                                              /* Initialize uC/OS-II                      */
    

    PC_DOSSaveReturn();                                    /* Save environment to return to DOS        */
    PC_VectSet(uCOS, OSCtxSw);                             /* Install uC/OS-II's context switch vector */

    OSTCBPrioTbl[OS_IDLE_PRIO]->deadline = 1002;
    OSTaskCreate(TaskStart, (void *)0, &TaskStartStk[TASK_STK_SIZE - 1], 0);    
    OSTCBPrioTbl[0]->deadline = 1;
    
    TaskStartCreateTasks();
    ArgumentSet3();
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
    //PC_SetTickRate(OS_TICKS_PER_SEC);                      /* Reprogram tick rate                      */
    PC_SetTickRate(1u);
    OS_EXIT_CRITICAL();

    //OSStatInit();                                          /* Initialize uC/OS-II's statistics         */
    //TaskStartCreateTasks();                                /* Create all the application tasks         */	
    //ArgumentSet();

    OSTimeSet(0);
    for (;;) {
        PrintCtxSwMessage();

        if (PC_GetKey(&key) == TRUE) {                     /* See if key has been pressed              */
            if (key == 0x1B) {                             /* Yes, see if it's the ESCAPE key          */
                PC_DOSReturn();                            /* Return to DOS                            */
            }
        }

        OSCtxSwCtr = 0;                                    /* Clear context switch counter             */
        //OSTimeDlyHMSM(0, 0, 1, 0);                       /* Wait one second                          */
        OSTimeDly(200);                                    /* Delay and wait (P-C) times               */
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

    //TaskData[0].exeTime = 1;
    //TaskData[0].period = 3;
    //TaskData[1].exeTime = 3;
    //TaskData[1].period = 6;

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

void Task (void *pdata)
{
    int start;   // start time
    int end;     // end time
    int toDelay;
    int c = OSTCBCur->compTime; 
    //struct period *tmpPdata = (struct period*) pdata;
    //int c = tmpPdata->exeTime;

    //OS_ENTER_CRITICAL();
    //OSTCBCur->compTime=c;                         // set the counter (c ticks for computation)
    //OSTCBCur->period=tmpPdata->period;               // set the period
    //OS_EXIT_CRITICAL();

    start = 0;
    while(1)
    {
        while(OSTCBCur->compTime > 0)             // C ticks
        {
            // do nothing
        }
        end = OSTimeGet();
        toDelay = (OSTCBCur->period) - (end-start);
        if (end > start+OSTCBCur->period){
            printf("time:%d task:%d exceed deadline\n", end, OSTCBCur->OSTCBPrio);
        }
        
        //printf("Task ends. time %d, end time %d, toDelay %d, prio%d, deadline%d\n", (int)start, (int)end, (int)toDelay, (int)OSTCBCur->OSTCBPrio, (int)OSTCBCur->deadline);
        
        OS_ENTER_CRITICAL();
        start = start + (OSTCBCur->period);
        OSTCBCur->compTime = c;                     // reset the counter (c ticks for computation)
        OSTCBCur->deadline = start + OSTCBCur->period;
        OS_EXIT_CRITICAL();
        OSTimeDly(toDelay);                         // delay and wait (P-C) times
    }
}

void ArgumentSet2(void){
    OS_TCB    *ptcb;
    ptcb = OSTCBList;
    while(ptcb->OSTCBPrio == 1 || ptcb->OSTCBPrio == 2 ){
        if(ptcb->OSTCBPrio == 1){
            ptcb->compTime = 1;
            ptcb->period = 3;
            ptcb->deadline = 3;
        }
        else if(ptcb->OSTCBPrio == 2){
            ptcb->compTime = 3;
            ptcb->period = 5;
            ptcb->deadline = 5;
        }
        ptcb = ptcb->OSTCBNext;
    }
}

void ArgumentSet3(void){
    OS_TCB    *ptcb;
    ptcb = OSTCBList;
    while(ptcb->OSTCBPrio == 1 || ptcb->OSTCBPrio == 2 || ptcb->OSTCBPrio == 3){
        if(ptcb->OSTCBPrio == 1){
            ptcb->compTime = 1;
            ptcb->period = 4;
            ptcb->deadline = 4;
        }
        else if(ptcb->OSTCBPrio == 2){
            ptcb->compTime = 2;
            ptcb->period = 5;
            ptcb->deadline = 5;
        }
        else if(ptcb->OSTCBPrio == 3){
            ptcb->compTime = 2;
            ptcb->period = 10;
            ptcb->deadline = 10;
        }
        ptcb = ptcb->OSTCBNext;
    }
}

void PrintCtxSwMessage(void){
    static int i;
    for(; i < CtxSwMessageTop; i++){
        printf("%s", CtxSwMessage[i]);
    }
}
