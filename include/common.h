/***********************************
 * 文件名称：common.h
 * 源 文 件：NULL
 * 功    能：包含常用C++库, 宏定义和常用数据结构
 ***********************************/

#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>//标准输入输出 “standard input & output"（标准输入输出）
#include <iostream>//标准输入输出流
#include <stdlib.h>//standard library标准库函数 头文件
#include <string.h>//字符数组的函数定义的头文件，strlen、strcmp、strcpy等等
#include <unistd.h>//对 POSIX 操作系统 API 的访问功能的头文件，操作系统如 Unix 的所有官方版本，包括 Mac OS X、Linux 等
#include <cmath>//C++数学运算的库函数
#include <time.h>//日期和时间头文件。用于需要时间方面的函数。
#include <pthread.h>//实现 POSIX 线程标准的库常被称作Pthreads，一般用于Unix-likePOSIX 系统如Linux、Solaris。可参考Linux多线程编程
#include <sys/types.h>//是Unix/Linux系统的基本系统数据类型的头文件，含有size_t，time_t，pid_t等类型。
#include <sys/mman.h>//将一个文件或者其它对象映射进内存，<sys/mman.h>是Unix头文件

#define DEBUG
#define ON 1
#define OFF 0
#define INIT_C 0
#define EXIT_C 255
#define Rad2Deg 180.0/M_PI
#define Deg2Rad M_PI/180.0

// 线程标志
struct shm_interface{
  int status_print;    // 打印状态信息标志(未使用)
  int status_control;  // 线程结束标志
};

// 标志位
typedef struct{
  int ServoFlag;  // 伺服启用标志
  int RscvFlag;   // 图像处理线程标志
  int ForceFlag;  // 力控标志
  int ReachFlag;  // 到达目标位置标志
}FLAG;

// 时间
typedef struct{
  double Curtime;
  double Rscv_time;
}TIME;

// 物体运动参数
typedef struct{
  double Curh;
  double Lasth;
  double Refh;
  double dhm;
  double d2hm;
}MOTION;

// 夹具状态
typedef struct{
	double Curpos;    // 当前位置
	double Refpos;    // 目标位置
  double Refforce;  // 期望压力值
  double Curforce;  // 实际压力值
}STATE;

// 临时数据
typedef struct{
  double temp;
}TEMP;

// 全局的共享数据结构体
typedef struct{
  MOTION Motion;
  FLAG   Flag;
  TIME   Time;
  STATE  State;
  TEMP   Temp;
}SVO;

#endif

