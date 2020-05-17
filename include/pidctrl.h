
#ifndef PIDCTRL_H
#define PIDCTRL_H

#include<cmath>    // C++中的数学库

// 位置式PID控制参数
typedef struct{
  double SetPoint;    // 设定值
  double ActPoint;    // 实际值
  double err;
  double err_last;
  double kp, ki, kd;
  double u;          // 反馈值
  double omn_err;    // 误差积分

  double u_max;    // 积分上限
  double u_min;    // 积分下限
}PID;

/***********************************
 * 函数名称：
 * 功    能：
 * 参    数：
 * 返 回 值：
 ***********************************/
void PID_Arg_Init(PID* pid, double initval);

double PID_Ctrl(PID* pid, double curval, double goal);

#endif

