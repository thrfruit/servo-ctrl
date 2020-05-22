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
  pid->ki       = -1*ki;
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

  /* 接收输入参数 */
  pid->SetPoint = goal;
  pid->ActPoint = curval;


  /* 比例项 */
  pid->err = pid->ActPoint - pid->SetPoint;

  /* 积分项 */
  if(std::abs(pid->err) > 150) {   // 误差过大时停用积分项和微分项
    fi = 0;
    fd = 0;
  }
  else {
    fi = 1;
    fd = 1;
    // 输出信号达到上限，只计算正积分 (因为PID系数为负)
    if(pid->u-pid->u_max>0) {
      if(pid->err > 0) {
        pid->omn_err += pid->err;
        // pid->omn_err += (pid->err + pid->err_last)/2;    // 梯形积分
      }
    }
    // 输出信号达到下限，只计算负积分 (因为PID系数为负)
    else if (pid->u-pid->u_min<0) {
      if(pid->err < 0) {
        pid->omn_err += pid->err;
        // pid->omn_err += (pid->err + pid->err_last)/2;
      }
    }
    else{
      pid->omn_err += pid->err;
      // pid->omn_err += (pid->err + pid->err_last)/2;
    }
  }
  
  /* 微分项*/
  pid->diff_err = pid->err - pid->err_last;
  pid->err_last = pid->err;

  /* 增量式PID控制 */
  // if(std::abs(pid->err) < 50) {    // 在误差允许范围内时保持输出量不变
  //   return pid->u;
  // }
  pid->u += pid->kp*pid->err + fi*pid->ki*pid->omn_err + fd*pid->kd*pid->err_last;

  return pid->u;
}

