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
int cnt = 0;
// 二阶临界阻尼系统参数
float wn = 0.5;
float goal = -20;
extern pthread_mutex_t mymutex;       // Shanw互斥锁
extern pthread_cond_t rt_msg_cond;    // Shawn条件变量
double force_0;

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
      exp_t = std::exp(-wn*curtime);
      hm = goal*(1-exp_t*(1+wn*curtime))/1000;
      dhm = goal*wn*wn*curtime*exp_t/1000;
      d2hm = goal*wn*wn*(1-wn*curtime)*exp_t/1000;
      servo_svo.Motion.Refh = hm;
      servo_svo.Motion.dhm  = dhm;
      servo_svo.Motion.d2hm = d2hm;

      // 读取压力值
      servo_svo.Curforce = getForce();

      // PID控制
      cmd_pos = PID_Ctrl(&pid, servo_svo.Curforce, servo_svo.Refforce);
      servo_svo.Refpos = cmd_pos;
      servo_svo.pidfit = pid.fit;

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
  double time;
  extern double kp, ki, kd;

  // 读取当前夹爪位置
  data->Refpos = rm_read_current_position(0);
  std::cout << "Curposition: " << data->Refpos << std::endl;

  // 读取当前压力值
  data->Refforce = getForce();
  force_0 = data->Refforce;
  std::cout << "Refforce = " << data->Refforce << std::endl;

  // 设置伺服标志
  data->ServoFlag = ON;
  data->ForceFlag = ON;
  
  if (data->ForceFlag == ON) {
    // 重置PID控制器
    PID_Arg_Init(&pid, kp, ki, kd, data->Refpos);
  }

  // 重置时间
  ResetTime();
  time = GetCurrentTime();
  SetStartTime(time);

  // 将数据同步到pSVO
  SvoWrite(data);

  /* *** 启用所有线程 *** */
  pthread_mutex_lock(&mymutex);
  pthread_cond_broadcast(&rt_msg_cond);
  pthread_mutex_unlock(&mymutex);
}


/*
 * 使用压力传感器测量压力值
 */
double getForce(void) {
  float adResult, adBuf[1024];
  double force;
  // 单次采集
  ADSingleV20(0, 0, &adResult);
  // 连续采集，一个数据包为512个数据
  // ADContinuV20(0, 0, 512, 100000, adBuf);
  // for(int i=0;i<512;i++) {
  //   adResult += adBuf[i];
  // }
  // adResult /= 512.0;
  force = (double)(100.0/(2.7-adResult)-40.0);
  return force;
}

