# RTOS

compile & run:  
1.goToSource  
2.clean.bat  
3.maketest.bat  
4.test.exe  

_________________________________________________________________________________

lab1 (RM Scheduler)修改的file  
test.C  
os_core.c :osintexit ,os_sched,osstart,ostimetick  
uCos.H :最下面的define,os_tcb  

_________________________________________________________________________________

lab2 (EDF Scheduler)修改的file  
test.C  
os_task.c 的ostaskcreate 把os_sched()註解   
(因為在pcb裡面加deadline -> 所以在taskcreate的時候deadline還沒有初始化,此時呼叫os_sched()會選錯對象)    
os_core.c ,osintexit ,os_sched,osstart,getPrioHightRdy,ostimetick  
uCos.H 最下面的define,os_tcb  

_________________________________________________________________________________

lab3 (Ceiling Priority Protocol + RM Scheduler)修改的file  
test.C  
OS_MUTEX.C 的 OSMutexPend , OSMutexPost  

_________________________________________________________________________________
