/***********************************
 * 文件名称：servo.h
 * 源 文 件：servo.cpp, main.cpp
 * 功    能：包含Linux多线程编程的C++库;
 *           伺服控制的基本设置;
 *           声明各子线程函数;
 ***********************************/

#ifndef SERVO_H
#define SERVO_H

/*shared memory header*/
#include <errno.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "common.h"
#include "system.h"
#include "rmctrl.h"
#include "usb-daq-v20.h"
#include "rscv.h"
#include "dataexchange.h"

/* We use 49 as the PRREMPT_RT use 50 as the priority of 
 * kernel tasklets and interrupt handler by default.
 * Priority range: 1-99 */
#define MY_PRIORITY (49)
/* The maximum stack size which is guaranteed safe
 * to access without faulting */
#define MAX_SAFE_STACK (8 * 1024)
/* The number of nsecs between two servo thread */
#define SERVO_INTERVAL (10000000)

typedef struct {
  double curpos;    // 夹具实际位置
  double refpos;    // 夹具期望位置
  double curforce;  // 实际压力值
  double refforce;  // 期望压力值
  double time;      // 时间
}SERVO;

/* =================== main.cpp ================== */
/***********************************
 * 函数名称：collect_function
 * 功    能：采集传感器信号的线程
 * 参    数：*param
 * 返 回 值：void
 ***********************************/
void *collect_function(void *param);

/***********************************
 * 函数名称：interface_function
 * 功    能：用户交互线程, 显示菜单、提示等
 * 参    数：*param
 * 返 回 值：void
 ***********************************/
void *interface_function(void *param);

/***********************************
 * 函数名称：DisplayMenu
 * 功    能：显示菜单
 * 参    数：NULL
 * 返 回 值：void
 ***********************************/
void DisplayMenu();

/***********************************
 * 函数名称：DisplayCurrentInformation
 * 功    能：打印系统当前状态信息
 * 参    数：NULL
 * 返 回 值：void
 ***********************************/
void DisplayCurrentInformation();


/* ================== servo.cpp ================== */
/***********************************
 * 函数名称：servo_function
 * 功    能：伺服线程
 * 参    数：RmDriver 夹爪类
 * 返 回 值：void
 ***********************************/
void servo_function(RmDriver *rm);

/***********************************
 * 函数名称：SetSvo
 * 功    能：完成伺服开始前的设置
 * 参    数：SVO 当前线程更新后的共享数据
 * 返 回 值：void
 ***********************************/
void SetSvo(SVO *data);

/***********************************
 * 函数名称：getForce
 * 功    能：使用压力传感器测量当前压力值
 * 参    数：NULL
 * 返 回 值：double 当前压力值
 ***********************************/
double getForce(void);

/***********************************
 * 功    能：数据保存
 ***********************************/
void ServoSaveDataReset();
void ServoDataSave(SERVO *data);
void ServoDataWrite();

#endif

