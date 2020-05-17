
#include "../include/common.h"
#include "../include/interface.h"
#include "../include/motion_axis.h"
#include "../include/servo.h"
#include "../include/system.h"
#include "../include/trajectory.h"
#include "../include/usb-daq-v20.h"
/*shared memory header*/
#include <errno.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <vector>
/*shared memory header*/
#include <termio.h> //gyh 用于读取键盘按键,不可在common.h中引用，否则会使servo.cpp中报错，未解

#define MY_PRIORITY (49)
#define MAX_SAFE_STACK (8 * 1024)

void *display_function(void *param);
void *interface_function(void *param);
void DisplayMenu(void);
void DisplayCurrentInformation(PATH path, int flag);

struct shm_interface shm_servo_inter; //用于控制所有线程的结束--shm_servo_inter.status_control
                                      //= EXIT_C三个线程都会结束
pthread_mutex_t servo_inter_mutex = PTHREAD_MUTEX_INITIALIZER; //定义共享内存时用到的 互斥锁-全局
// PTHREAD_MUTEX_INITIALIZER为 宏定义常量： { { 0, 0, 0, 0, 0, 0, { 0, 0 } } }

int shmid; // 共享内存标识符
void *shm_addr = NULL; //存放共享内存的起始地址，用于后面的共享内存操作
SVO key2intfc_svo; // gyh
extern SVO pSVO; //!!最主要的一个变量，结构体SVO的定义在common.h中!!，该变量定义在dataExachange.cpp中，存在于这两个文件中
pthread_mutex_t mymutex; //??这个extern什么意思，没在别的文件中找到变量mymutex的定义
pthread_cond_t rt_msg_cond;

/* for test */
void stack_prefault(void) {
  unsigned char dummy[MAX_SAFE_STACK];
  memset(dummy, 0, MAX_SAFE_STACK); //初始化dummy[]所在的内存空间
  return;
}

int main(void) {

  pthread_t interface_thread, display_thread; //创建线程标识符
  int loop_flag; //循环标志，在后面死循环中用到
  loop_flag = 0;

  shm_servo_inter.status_control = INIT_C; // INIT_C=0
  struct timespec t; //，用于时间控制，linux系统的高精度时间变量，两个成员：秒 和 纳秒
  struct sched_param param; //描述线程调度参数，获得或设置线程的调度参数时，会使用sched_param结构
  int interval = 8000000;   /* 8 ms*/ // 8000000纳秒，后面用于加给 t.tv_nsec(Nanosecond)

  /* connect to ur robot */
  printf("\nExperiment Start\n");
  SaveDataReset();

  // Init RmClawer
  rm_init();
  rm_axis_handle handle = rm_open_axis_modbus_rtu("/dev/ttyS110", 115200, 0);
  rm_reset_error(handle);
  rm_go_home(handle);
  rm_push(handle, 10, 7.5, 10);
  // rm_move_absolute(handle, 2, 10, 3000, 3000, 0.1);
  rm_config_motion(handle, 100, 3000, 3000);

  sleep(1);
  pSVO.Refpos = rm_read_current_position(handle);
  std::cout << "Curposition" << pSVO.Refpos << std::endl;

  // Init UsbV20
  if (-1 == OpenUsbV20()) {
    printf("...... UsbV20! Open! Failed! ......");
  }

  /* Declare ourself as a real time task */
  param.sched_priority = MY_PRIORITY; // sched_priority代表分配给进程的优先级，此处设置为49
                   // //スケジューラでのプロセスの優先順位を変更する(99が優先順位最大)
  if (sched_setscheduler(0, SCHED_FIFO, &param) == -1) //设置线程的调度策略和调度参数
  { //第一个参数是0表示要设置调度参数的目标线程调用该函数的线程-就是这个mian函数线程
    perror("sched_setscheduler failed\n");
    exit(-1);
  }

  /* Lock memory */
  if (mlockall(MCL_CURRENT | MCL_FUTURE) == -1) {//锁住内存,避免这段内存被操作系统调度到swap空间，mlockall不仅锁定当前的地址空间，也要锁定将来申请的地址空间
    perror("mlockall failed");
    exit(-2);
  }

  stack_prefault(); //申请一块8*1024大小的内存并初始化，且由于上面的操作，这块内存是被锁住的、独占的，--其实就是新建并初始化了个数组
  clock_gettime(CLOCK_MONOTONIC, &t); //读取当前精确时间给t
                                      /* start after one second */
  t.tv_sec++;                         //一秒后启动控制回路
              ////t.tv_sec++，然后后面第一次进入循环时，clock_nanosleep()函数就会睡到当前时间加一秒，也就是睡一秒（当然，这中间隔这么多行程序的运行时间就忽略了）

  /* 启动另外两个线程 */
  /* start interface thread */
  if (pthread_create(&interface_thread, NULL, interface_function, NULL)) //启动线程interface_thread，第三个参数是线程运行函数（interface_function）
  {
    perror("interface_thread create\n");
    exit(1);
  }

  /* start display thread */
  if (pthread_create(&display_thread, NULL, display_function, NULL)) {//同上
    perror("Display_thread create\n");
    exit(1);
  }

  /* reset save buffer */
  SaveDataReset(); //实验数据的存储有关，初始化 控制往文件里写数据 的变量
  /*创建共享内存*/
  shmid = shmget((key_t)1234, 72, 0666 | IPC_CREAT); //共享内存段名称为1234,长度为72，0666表示读写权限，创建成功返回这段内存的标志，失败返回-1
  if (shmid == -1) {
    fprintf(stderr, "shmat failed\n");
    exit(EXIT_FAILURE); //失败地退出函数
  }
  /*创建共享内存*/
  /*将共享内存连接到当前进程的地址空间(映射共享内存)*/
  shm_addr = (void *)shmat(shmid, 0, 0);

  while (1) {
    if (shm_servo_inter.status_control == EXIT_C) {
      printf("Program end\n");
      /*分离共享内存*/
      if (shmdt(shm_addr) == -1) {
        printf("shmdt error!\n");
        exit(1);
      }
      /*分离共享内存*/
      break;
    }

    /* wait until next shot */
    clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL); //绝对睡眠，基于CLOCK_MONOTONIC时钟，睡眠到参数t指定的时间。
    //在这里设定执行到特定时刻时将执行权限交给其他的进程．
    //時刻が来たら実行権限を復活させて優先的に実行される
    /* do the stuff */
    /* reset */
    if (loop_flag == 0){ //循环进行第一次时，初始化时间
      ResetTime();
      loop_flag = 1;
    }

    servo_function();

    /* calculate next shot */
    t.tv_nsec += interval; //次にプロセスが復帰する時刻を設定  // tv_nsec ：纳秒

    while (t.tv_nsec >= NSEC_PER_SEC) //时间进制，保证时间纳秒加到上限就进位
    {                                 //桁の繰り上げ
      t.tv_nsec -= NSEC_PER_SEC;
      t.tv_sec++;
    }
  } //死循环结尾，这个死循环其实就一直在干一件事，睡8ms让机械臂动一下，一遍一遍的睡8ms动一下

  /* 运行到这，说明上面的死循环结束了，即用户选择了退出 */
  if (pthread_join(interface_thread, NULL)) {//用于等待其他线程结束：当调用 pthread_join()
                 //时，当前线程会处于阻塞状态，直到被调用的线程结束后，当前线程才会重新开始执行。
    perror("pthread_join at interface_thread\n");
    exit(1);
  }
  if (pthread_join(display_thread, NULL)) {//等待display线程结束
    perror("pthread_join at displya_thread\n");
  }

  // close IO
  ExpDataWrite(); //存数据，往文件里写刚才存在结构体变量Exp_data里的数据

  // 终止程序
  exit(1);
}

/* display线程运行的函数，用于交互界面--就是往屏幕上显示信息*/
/* 如果开始了伺服（ServoFlag=ON） */
/* 开始位置伺服运动时(第一个if)：刷刷刷地疯狂显示【现在的时间、现在的末端位姿、目标的末端位姿】
 */
/* 开始关节角度伺服运动时(第二个if)：显示的是
 * 【现在的时间、现在的关节角度、目标的关节角度】 */
void *display_function(void *param) {
  int status_interface, i; // status_interface后面就没用到，i用于循环
  int loop_counter;        //这个后面没用到
  SVO display_svo;         //这是最主要的存放数据的变量
  double time;
  double jnt_angle[6]; //用来存放最后算出来的角度

  loop_counter = 0; //没用

  do {
    int loop_counter;
    double time;
    double pos;
    SVO display_svo;

    loop_counter = 0;

    pthread_mutex_lock(&mymutex);
    pthread_cond_wait(&rt_msg_cond, &mymutex);
    pthread_mutex_unlock(&mymutex);

    do {
      time = GetCurrentTime();
      SvoReadFromDis(&display_svo); // Read data
      if ((display_svo.ServoFlag == ON) &&
          (GetOffsetTime() < (1.0 / (double)display_svo.Path.Freq))) {
        printf("Time:%0.1f\n", time);
        printf("Current position of claw: %.2f\n", display_svo.Curpos);
        printf("Reference position of claw: %.2f\n", display_svo.Refpos);
        printf("Curforce signal: %.2f\n", display_svo.Curforce);
      }
      usleep(25000); // Delay for 25ms
    } while (shm_servo_inter.status_control != EXIT_C);
    printf("End of Display Function.\n");
    return ((void *)0);
  } while (shm_servo_inter.status_control !=
           EXIT_C); //只要用户没退出，这个线程就一直在这里死循环着
  printf("\nEnd of Display Function\n");
  return ((void *)0);
}

/* interface线程运行的函数，用于选择操作、输入参数 */
void *interface_function(void *param) {

  int interface_counter; //记录下面循环的次数
  interface_counter = 0;
  int end = 1; //该线程函数的结束标志，用户选择e:退出，则归0，退出循环
  int command, i;

  SVO interface_svo; //存放在这个线程中 产生的机器人控制信息，最后都会传给pSVO

  double c;
  printf("Executing interface function\n");

  DisplayMenu(); //显示选项信息

  do {

  int interface_counter;
  interface_counter = 0;
  int end = 1;
  int command;
  SVO interface_svo;

  SvoRead(&interface_svo);

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
      case 'f':
      case 'F':
        printf("Force:\n");
        scanf("%lf", &interface_svo.Refforce);
        interface_svo.ForceFlag = ON;
        interface_svo.PathFlag = OFF;
        break;
      case 'p':
      case 'P':
        printf("----------------- Now you are in SvoMode -----------------\n");
        printf("Set the goal position:\n");
        ChangePosData(&interface_svo.Path);
        interface_svo.ForceFlag = OFF;
        interface_svo.PathFlag = ON;
        break;
      case 's':
      case 'S':
        printf("Start\n");
        SetSvo(&interface_svo);
        pthread_cond_signal(&rt_msg_cond);
        break;
      case 'i':
      case 'I':
        SvoRead(&interface_svo);
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
  sleep(4);
  // rm_close_axis(0);
  printf("End of Experiment\n");
  return 0;
  } while (end);
  sleep(5);
  printf("End of Experiment\n");
}

void DisplayMenu(void) {
  printf("\n**************Menu (please input [CR])*****************\n");
  printf("CurPosOfHnd:*************[c : C]\n");
  printf("Gain:********************[g : G]\n");
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
  printf("Current Force[N]: %.2f\n", pSVO.Curforce);
}

