/****************************************************
 * TODO:
 * 1. 添加变积分过程，加快消除静差的速度。
 * 2. 添加可接受误差。
 * 3. 压力预警。
 * *************************************************/

#include"../include/pidctrl.h"

void PID_Arg_Init(PID* pid, double initval){
  pid->kp       = -0.1;
  pid->ki       = -0.08;
  pid->kd       = 0;

  pid->SetPoint = 0;
  pid->ActPoint = 0;
  pid->u        = initval;
  pid->err      = 0;
  pid->err_last = 0;
  pid->omn_err  = 0;

  pid->u_max    = 10;
  pid->u_min    = 2;
}

double PID_Ctrl(PID* pid, double curval, double goal){
  int kc;    // 积分分离标志

  /* 接收输入参数 */
  pid->SetPoint = goal;
  pid->ActPoint = curval;


  /* 比例项 */
  pid->err = pid->ActPoint - pid->SetPoint;

  /* 积分项 */
  if(std::abs(pid->err) > 0.2) {   // 误差过大时停用积分项
    kc = 0;
  }
  else if(std::abs(pid->err) < 0.02) {    // 在误差允许范围内时返回当前输出量
    return pid->u;
  }
  else {
    kc = 1;
    // 输出信号达到上限，只计算负积分
    if((pid->ActPoint-pid->u_max>0) & (pid->err<0)) {
      // pid->omn_err += pid->err;
      pid->omn_err += (pid->err + pid->err_last)/2;    // 梯形积分
    }
    // 输出信号达到下限，只计算正积分
    else if ((pid->ActPoint-pid->u_min<0) & (pid->err>0)) {
      // pid->omn_err += pid->err;
      pid->omn_err += (pid->err + pid->err_last)/2;
    }
    else{
      pid->omn_err += (pid->err + pid->err_last)/2;
    }
  }
  
  /* 微分项*/
  pid->err_last = pid->err;

  /* 增量式PID控制 */
  pid->u += pid->kp*pid->err + kc*pid->ki*pid->omn_err;

  return pid->u;
}

