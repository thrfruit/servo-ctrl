/*
 * POSIX Real Time Example
 * using a single pthread as RT thread
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
#include"../include/motion_axis.h"
#include"../include/common.h"
#include"../include/servo.h"
#include"../include/system.h"
#include"../include/interface.h"
#include"../include/trajectory.h"
/*** Personal headers ***/

/* we use 49 as the PRREMPT_RT use 50
 * as the priority of kernel tasklets
 * and interrupt handler by default */
#define MY_PRIORITY (49)
/* The maximum stack size which is guaranteed
 * safe to access without faulting */
#define MAX_SAFE_STACK  (8*1024)
#define NSEC_PER_SEC (1000000000)   /* The number of nsecs per sec. */

/*** Declare of user defined function ***/
void *interface_function(void* param);
void *display_function(void *param);
void DisplayMenu(void);
void DisplayCurrentInformation(PATH path, int flag);
extern SVO pSVO;
/*** Declare of user defined function ***/

/* Macro definition */
struct shm_interface shm_servo_inter;
pthread_mutex_t servo_inter_mutex = PTHREAD_MUTEX_INITIALIZER;
/* Macro definition */

/* Pre-fault our task */
void stack_prefault(void) {
  unsigned char dummy[MAX_SAFE_STACK];
  memset(dummy, 0, MAX_SAFE_STACK); //初始化dummy[]所在的内存空间
  return;
}

int main(int argc, char *argv[]) {
  struct timespec t;
  struct sched_param param;
  int interval = 8000000; /* 8 ms*/

  /*** At beginning ***/
  printf("Executing main function\n");
  pthread_t interface_thread, display_thread; // Declare threads;

  rm_init();
  rm_axis_handle handle = rm_open_axis_modbus_rtu("/dev/ttyS110", 115200, 0);
  rm_reset_error(handle);

  rm_go_home(handle);
  while (rm_is_moving(handle));
  printf("RM is home\n");

  // rm_move_absolute(handle, 10, 10, 3000, 3000, 0.1);
  // rm_push(handle, 50, 10, 10);
  // while (rm_is_moving(handle));

  /*** At beginning ***/

  /* Declare ourself as a real time task */
  param.sched_priority = MY_PRIORITY;
  if (sched_setscheduler(0, SCHED_FIFO, &param) == -1) {
    perror("sched_setscheduler failed\n");
    exit(-1);
  }

  /* Lock memory */
  if (mlockall(MCL_CURRENT | MCL_FUTURE) == -1) {
    perror("mlockall failed");
    exit(-2);
  }

  stack_prefault();
  clock_gettime(CLOCK_MONOTONIC, &t);

  /* start after one second */
  t.tv_sec++; //一秒后启动控制回路

  /*** Create personal threads ***/
  if (pthread_create(&interface_thread, NULL, interface_function, NULL)) {
    perror("interface_thread create\n");
    exit(1);
  }
  if (pthread_create(&display_thread, NULL, display_function, NULL)) {
    perror("display_thread create\n");
    exit(1);
  }
  /*** Create personal threads ***/

  while (1) {
    /* wait until next shot */
    clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);

    /* do the stuff */
    servo_function();

    /* do the stuff */

    /* calculate next shot */
    t.tv_nsec += interval;

    while (t.tv_nsec >= NSEC_PER_SEC) {
      t.tv_nsec -= NSEC_PER_SEC;
      t.tv_sec++;
    }
  }
}

/*** Define function ***/
//
void *interface_function(void *param) {

  int interface_counter;
  interface_counter = 0;
  int end = 1;
  int command;
  SVO interface_svo;

  printf("Executing interface function\n");
  DisplayMenu();
  do {
    // Display the current of the robot:
    // Wait command
    printf("Please hit any key\n");

    command = getchar();
    switch (command) {
    case 'c':
    case 'C':
      printf("Hello, this function is not finished.\n");
      break;
    case 'p':
    case 'P':
      printf("----------------- Now you are in SvoMode -----------------\n");
      printf("Set the goal position:\n");
      ChangePosData(&interface_svo.Path);
      break;
    case 's':
    case 'S':
      printf("Start\n");
      SetSvo(&interface_svo);
      break;
    case 'i':
    case 'I':
      DisplayCurrentInformation(interface_svo.Path, interface_svo.ServoFlag);
      break;
    case 'e':
    case 'E':
      end = 0;
      shm_servo_inter.status_control = EXIT_C;
      break;
    default:
      DisplayMenu();
      break;
    }
    interface_counter++;
  } while (end);
  sleep(1);
  rm_close_axis(0);
  printf("End of Experiment\n");
  return 0;
}

void *display_function(void *param) {
  int loop_counter;
  double time;
  double pos;
  SVO display_svo;

  loop_counter = 0;

  do {
    time = GetCurrentTime();
    SvoReadFromDis(&display_svo); // Read data
    if ((display_svo.ServoFlag == ON) &&
        (GetOffsetTime() < (1.0 / (double)display_svo.Path.Freq))) {
      printf("Time:%0.1f\n", time);
      printf("Current position of claw: %.2f\n", display_svo.Curpos);
      printf("Reference position of claw: %.2f\n", display_svo.Refpos);
    }
    usleep(25000); // Delay for 25ms
  } while (shm_servo_inter.status_control != EXIT_C);
  printf("End of Display Function.\n");
  return ((void *)0);
}

//
void DisplayMenu(void) {
  printf("\n**************Menu (please input [CR])*****************\n");
  printf("CurPosOfHand:*************[c : C]\n");
  printf("Path:********************[p : P]\n");
  printf("Start:*******************[s : S]\n");
  printf("Info:********************[i : I]\n");
  printf("Destination:*************[d : D]\n");
  printf("End:*********************[e : E :ESC]\n");
}

void DisplayCurrentInformation(PATH path, int flag){
  printf("----------------- Current Information -------------------\n");
  printf("Path frequency = %f [HZ]\n", pSVO.Path.Freq);
  printf("Current position[mm]: %.2f\n", pSVO.Curpos);
  printf("Goal position[mm]: %.2f\n", pSVO.Path.Goal);
}

