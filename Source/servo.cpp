/***********************************
 * 文件名称：servo.cpp
 * 头 文 件：servo.h
 * 功    能：伺服线程的实现;
 ***********************************
 * TODO:
 * *********************************/

#include "../include/servo.h"
#include "../include/common.h"
#include "../include/trajectory.h"
#include "../include/pidctrl.h"

PID pid;
float adBuf[1024];
float adResult = 0.0;
int cnt = 0;

void servo_function(RmDriver *rm) {
  int ret, i;
  SVO servo_svo;
  PATH path;
  double curtime;
  double robot_pos, cmd_pos;
  double start, end;    // shawn: calculate waste time
  double exp_t, hm, dhm, d2hm;

  /* Get the current status */
  SvoRead(&servo_svo);
  // Copy the status of Rm
  // robot_pos = rm.getPos();
  robot_pos = servo_svo.Refpos;
  servo_svo.Curpos = robot_pos;
  // Get current time
  curtime = GetCurrentTime();
  servo_svo.Time = curtime;

  // Set servo data
  // Note: 这里保存数据最好先判断伺服标志是否置位，否则有可能Setsvo函数无法取得线程，
  // 导致开始时无法传递伺服运动参数。但是这样做的话，无法在伺服未启动时读取夹具状态。
  if (servo_svo.ServoFlag == ON) {

    if (servo_svo.ForceFlag == ON) {
      // 计算物体期望运动状态
      exp_t = std::exp(-2*curtime);
      hm    = 20*(1- exp_t*(1+2*curtime));
      dhm   = 20*4*curtime*exp_t;
      d2hm  = 20*4*(1-2*curtime)*exp_t;
      servo_svo.Motion.Refh = hm;
      servo_svo.Motion.dhm  = dhm;
      servo_svo.Motion.d2hm = d2hm;

      // 读取压力值
      ADSingleV20(0, 0, &adResult);    // 单次采集
      // ADContinuV20(0, 0, 512, 100000, adBuf);    // 连续采集，一个数据包为512个数据
      // end = GetCurrentTime();
      // for(i=0;i<512;i++) {
      //   adResult += adBuf[i];
      // }
      // adResult /= 512.0;
      servo_svo.Curforce = (double)(100/(2.7-adResult)-40);

      // PID控制
      cmd_pos = PID_Ctrl(&pid, servo_svo.Curforce, servo_svo.Refforce);
      servo_svo.Refpos = cmd_pos;

      /* 发起运动指令 */
      if(servo_svo.Curforce > 450) {cmd_pos = 0;}    // 压力预警
      rm->setPos(cmd_pos);
    }  // if (ForceFlag)


    /* 保存数据到公共数据库 */
    SvoWrite(&servo_svo);

    /* 将数据保存到队列，用于存档 */
    ExpDataSave(&servo_svo);
  }  // if (ServoFlag)
  cnt++;
}

/*
 * 开始伺服控制。清空命令堆栈，将当前轨迹放入堆栈。
 */
void SetSvo(SVO *data) {
  int ret;
  double time;
  extern double kp, ki, kd;

  data->Refpos = rm_read_current_position(0);
  std::cout << "Curposition" << data->Refpos << std::endl;

  if (data->ForceFlag == ON) {
    PID_Arg_Init(&pid, kp, ki, kd, data->Refpos);    // 重置PID控制器
  }

  // 设置伺服标志
  data->ServoFlag = ON;
  data->ForceFlag = ON;
  
  // 重置时间
  ResetTime();
  time = GetCurrentTime();
  SetStartTime(time);

  SvoWrite(data);    // 将数据同步到pSVO
}

