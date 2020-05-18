/***********************************
 * 文件名称：system.h
 * 源 文 件：system.cpp
 * 功    能：高精度时间; 计时器;
 ***********************************/

#ifndef SYSTEM_H
#define SYSTEM_H

#include "./common.h"

/* The number of nsecs per sec. */
#define NSEC_PER_SEC (1000000000)

/***********************************
 * 函数名称：GetCurrentTime
 * 功    能：获得当前系统时间(s)
 * 参    数：NULL
 * 返 回 值：double 当前系统时间(s)
 ***********************************/
double GetCurrentTime();

/***********************************
 * 函数名称：ResetTime
 * 功    能：重置系统时间，用来记录程序运行的总时间
 *           及重要时间戳
 * 参    数：NULL
 * 返 回 值：void
 ***********************************/
void ResetTime();

/***********************************
 * 函数名称：GetOffsetTime
 * 功    能：获得当前计时器时间(s)
 * 参    数：NULL
 * 返 回 值：double 当前计时器时间(s)
 ***********************************/
double GetOffsetTime();

/***********************************
 * 函数名称：SetStartTime
 * 功    能：重置计时器，用来记录子程序的运行时间
 * 参    数：double 当前系统时间
 * 返 回 值：void
 ***********************************/
void SetStartTime(double time);

#endif

