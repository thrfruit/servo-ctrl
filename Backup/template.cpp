/*
 * POSIX Real Time Template using a single pthread as RT thread;
 * root, -lpthread,
 */

#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<sched.h>
#include<sys/mman.h>
#include<string.h>

/*** Personal headers ***/
#include<unistd.h>
#include<pthread.h>
/*** Personal headers ***/

/* We use 49 as the PRREMPT_RT use 50
 * as the priority of kernel tasklets
 * and interrupt handler by default.
 * Priority range: 1-99 */
#define MY_PRIORITY	(49)
/* The maximum stack size which is guaranteed
 * safe to access without faulting */
#define MAX_SAFE_STACK  (8*1024)
#define NSEC_PER_SEC (1000000000)   /* The number of nsecs per sec. */

/* Declare user defined function */
//void *interface_function(void* param);
/* Declare user defined function */

/* Pre-fault our task */
void stack_prefault(void)
{
    unsigned char dummy[MAX_SAFE_STACK];
    memset(dummy, 0, MAX_SAFE_STACK);   //初始化dummy[]所在的内存空间
    return;
}

int main(int argc, char* argv[])
{
    struct timespec t;
    struct sched_param param;
    int interval = 8000000; /* 8 ms*/

    /*** At beginning ***/
    printf("Executing main function\n");
    pthread_t interface_thread;     // Declare threads;
    /*** At beginning ***/

    /* Declare ourself as a real time task */
    param.sched_priority = MY_PRIORITY;
    if(sched_setscheduler(0,SCHED_FIFO, &param) == -1) {
        perror("sched_setscheduler failed\n");
        exit(-1);
     }

     /* Lock memory */
     if(mlockall(MCL_CURRENT|MCL_FUTURE) == -1) {
        perror("mlockall failed");
        exit(-2);
     }

     stack_prefault();
     clock_gettime(CLOCK_MONOTONIC ,&t);

     /* start after one second */
     t.tv_sec++;//一秒后启动控制回路

     /*** Create personal threads ***/
//     if(pthread_create(&interface_thread, NULL,interface_function, NULL))
//     {
//       perror("interface_thread create\n");
//       exit(1);
//     }
     /*** Create personal threads ***/

     while(1) {
         /* wait until next shot */
         clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);

         /* do the stuff */

         /* calculate next shot */
         t.tv_nsec += interval;

         while(t.tv_nsec >= NSEC_PER_SEC) {
             t.tv_nsec -= NSEC_PER_SEC;
             t.tv_sec++;
         }
     }
}


/*** Define function ***/

