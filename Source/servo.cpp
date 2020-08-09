/***********************************
 * 文件名称：servo.cpp
 * 头 文 件：servo.h
 * 功    能：伺服线程的实现;
 ***********************************
 * TODO:
 * *********************************/

#include "../include/servo.h"
#include "../include/common.h"
#include "../include/pidctrl.h"

PID pid;
int cnt = 0;
// 二阶临界阻尼系统参数
float wn = 0.5;
float goal = -20;
double force_0;
extern pthread_mutex_t mymutex;       // Shanw互斥锁
extern pthread_cond_t rt_msg_cond;    // Shawn条件变量
float adResult, adBuf[1024];
SVO servo_svo;
SERVO Servo;

void servo_function(RmDriver *rm) {
  double curtime;
  double curpos, cmd_pos;
  double exp_t, hm, dhm, d2hm;
  double curfn, ufn;

  /* *** Get the current status *** */
  SvoRead(&servo_svo);
  // Copy the status of Rm
  Servo.curpos = Servo.refpos;
  // Get current time
  curtime = GetCurrentTime();
  // Get the force sign
  ufn = servo_svo.State.Refforce;

  /* *** Do servo control *** */
  if (servo_svo.Flag.ServoFlag == ON) {
    // 根据参考模型计算物体期望运动状态
    exp_t = std::exp(-wn*curtime);
    hm = goal*(1-exp_t*(1+wn*curtime))/1000;
    dhm = goal*wn*wn*curtime*exp_t/1000;
    d2hm = goal*wn*wn*(1-wn*curtime)*exp_t/1000;

    // 读取压力值
    curfn = getForce();
    // PID控制
    cmd_pos = PID_Ctrl(&pid, curfn, ufn);
    // 压力预警
    if(curfn > 450) {cmd_pos = 0;}

    // 发起运动指令
    rm->setPos(cmd_pos);
  }  // if (ServoFlag)

  /* *** Update and Save Data *** */
  if (servo_svo.Flag.ServoFlag == ON) {
    servo_svo.Time.Curtime = curtime;
    servo_svo.Motion.Refh  = hm;
    servo_svo.Motion.dhm   = dhm;
    servo_svo.Motion.d2hm  = d2hm;
    Servo.time     = servo_svo.Time.Curtime;
    Servo.curforce = curfn;
    Servo.refforce = ufn;
    Servo.refpos   = cmd_pos;
    // 更新共享数据
    SvoWrite(&servo_svo);
    // 将数据保存到队列，用于存档
    ServoDataSave(&Servo);
    ExpDataSave(&servo_svo);

    // 累加伺服计数器
    cnt++;
  }  // if (DataUpdate)

  /* *** 线程同步 *** */
  // 每过6个伺服周期唤醒一次图像处理周期
  if (cnt == 6) {
    cnt = 0;
    pthread_mutex_lock(&mymutex);
    pthread_cond_signal(&rt_msg_cond);
    pthread_mutex_unlock(&mymutex);
  }
}

/*
 * 开始伺服控制。清空命令堆栈，将当前轨迹放入堆栈。
 */
void SetSvo(SVO *data) {
  double time;
  double refpos;
  double ufn;
  extern double kp, ki, kd;

  // 读取当前夹爪位置
  refpos = rm_read_current_position(0);
  std::cout << "Curposition: " << refpos << std::endl;

  // 读取当前压力值
  ufn = getForce();
  std::cout << "Refforce = " << ufn << std::endl;

  // 设置伺服标志
  data->Flag.ServoFlag = ON;
  
  // 重置PID控制器
  PID_Arg_Init(&pid, kp, ki, kd, refpos);
  
  // 重置时间
  ResetTime();
  time = GetCurrentTime();
  SetStartTime(time);

  // 同步数据
  Servo.refpos = refpos;
  data->State.Refforce = ufn;
  force_0 = ufn;
  SvoWrite(data);
}


/*
 * 使用压力传感器测量压力值
 */
double getForce(void) {
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

