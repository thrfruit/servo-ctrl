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
  float du;

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
  // else if(std::abs(pid->err) < 10) {    // 在误差允许范围内时保持输出量不变
  //     // pid->err = 0;
  //     return pid->u;
  //   }
  else{
    fi = 1;
    fd = 1;
  }
  
  /* 微分项*/
  pid->diff_err = pid->err - pid->err_last;
  pid->err_last = pid->err;

  /* 比例项 */
  if (pid->ActPoint > 220) {
    fp = 0.5;
  }
  else{
    fp = 1;
  }

  /* 增量式PID控制 */
  du = fp*pid->kp*pid->err + fd*pid->kd*pid->diff_err;
  pid->u += du;

  return pid->u;
}

