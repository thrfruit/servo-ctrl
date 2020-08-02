/***********************************
 * 文件名称：pidctrl.h
 * 源 文 件：pidctrl.cpp
 * 功    能：定义了包含PID控制参数的结构体;
 *           PID控制函数;
 ***********************************/

#ifndef PIDCTRL_H
#define PIDCTRL_H

#include<cmath>    // C++中的数学库

// PID控制参数
typedef struct{
  double SetPoint;    // 设定值
  double ActPoint;    // 实际值
  double err;         // 本次位置误差
  double err_last;    // 上次位置误差
  double kp, ki, kd;  // PID控制系数
  double u;           // 反馈值(输出信号)
  double omn_err;     // 误差积分
  double diff_err;    // 误差微分

  double u_max;       // 输出信号上限
  double u_min;       // 输出信号下限
  int fit;            // 达到合适的控制效果
}PID;

/***********************************
 * 函数名称：PID_Arg_Init
 * 功    能：初始化PID控制参数结构体
 * 参    数：PID结构体, 初始输出信号值
 * 返 回 值：void
 ***********************************/
void PID_Arg_Init(PID* pid, double kp, double ki, double kd, double initval);

/***********************************
 * 函数名称：PID_Ctrl
 * 功    能：根据位置型PID算法求需要的信号值
 * 参    数：PID结构体, 当前位置, 目标位置
 * 返 回 值：double 期望信号值
 ***********************************/
double PID_Ctrl(PID* pid, double curval, double goal);

#endif

