/***********************************
 * 文件名称：trajectory.h
 * 源 文 件：trajectory.cpp
 * 功    能：轨迹规划; 轨迹插补;
 ***********************************/

#ifndef TRAJECTORY_H
#define TRAJECTORY_H

#include "./common.h"

/* 轨迹插补的函数类型 */
#define PATH_SIN 0
#define PATH_5JI 5
#define PATH_3JI 3
#define PATH_1JI 1
#define TRJ_LENGTH 20  // 位移命令队列长度

// 位移命令队列
typedef struct {
  int data_num;
  PATH Path[TRJ_LENGTH];
} TRJ_BUFF;

/***********************************
 * 函数名称：initTrjBuff
 * 功    能：初始化位移命令队列
 * 参    数：NULL
 * 返 回 值：void
 ***********************************/
void initTrjBuff();

/***********************************
 * 函数名称：PutTrjBuff
 * 功    能：将一个位移命令存入队列
 * 参    数：PATH 位移
 * 返 回 值：int 存放成功标志(成功返回0，失败返回1)
 ***********************************/
int PutTrjBuff(PATH* path);

/***********************************
 * 函数名称：GetTrjBuff
 * 功    能：从队列取出一个位移命令
 * 参    数：PATH 用于存放位移命令的 PATH 结构体
 * 返 回 值：int 取值成功标志(成功返回0，失败返回1)
 ***********************************/
int GetTrjBuff(PATH* path);

/***********************************
 * 函数名称：Calc1JiTraje
 * 功    能：以一次函数进行轨迹插补
 * 参    数：double 位移起点, double 位移终点, double 计算频率(1/完成时间), double 运动时间
 * 返 回 值：double 目标位置
 ***********************************/
double Calc1JiTraje(double orig, double goal, double freq, double time);

/***********************************
 * 函数名称：CalcSinTraje
 * 功    能：以三角函数进行轨迹插补, 位移起始点设置无效
 * 参    数：double 运动频率, double 运动时间
 * 返 回 值：double 目标位置
 ***********************************/
double CalcSinTraje(double freq, double time);

/***********************************
 * 函数名称：CalcRefPath
 * 功    能：调用上述插值函数进行轨迹插补
 * 参    数：double 运动时间, PATH 位移, double 当前位置
 * 返 回 值：void
 ***********************************/
void CalcRefPath(double curtime, PATH *path, double *pos);

#endif

