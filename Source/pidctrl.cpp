/***********************************
 * 文件名称：pidctrl.cpp
 * 头 文 件：pidctrl.h
 * 功    能：实现PID控制
 ***********************************
 * TODO:
 * 添加变积分过程，加快消除静差的速度
 * *********************************/

#include "../include/pidctrl.h"

void PID_Arg_Init(PID* pid, double kp, double ki, double kd, double initval){
  pid->kp       = -1*kp;
  pid->ki       = 0;
  pid->kd       = -1*kd;

  pid->SetPoint = 0;
  pid->ActPoint = 0;
  pid->u        = initval;
  pid->err      = 0;
  pid->err_last = 0;
  pid->omn_err  = 0;
  pid->diff_err = 0;

  pid->u_max    = 10;
  pid->u_min    = 2;
}

double PID_Ctrl(PID* pid, double curval, double goal){
  int fi, fd;    // 积分, 微分分离标志
  float fp;      // 比例项分段系数
  double du;

  /* 接收输入参数 */
  pid->SetPoint = goal;
  pid->ActPoint = curval;


  /* 比例项 */
  pid->err = pid->ActPoint - pid->SetPoint;

  /* 微分项 */
  if(std::abs(pid->err) > 100) {   // 误差过大时停用积分项和微分项
    fd = 0;
  }
  else{
    fd = 1;
  }
  pid->diff_err = pid->err - pid->err_last;
  pid->err_last = pid->err;
  
  /* 输出信号 */
  // 输出信号增量
  du = pid->kp*pid->err + fd*pid->kd*pid->diff_err;
  // 限制输出范围
  if(pid->u+du > pid->u_max) {
    pid->u = pid->u_max;
  }
  else if(pid->u+du < pid->u_min) {
    pid->u = pid->u_min;
  }
  else{
    pid->u += du;
  }

  return pid->u;
}

