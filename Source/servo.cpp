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

void servo_function(RmDriver *rm) {
  int ret, i;
  SVO servo_svo;
  PATH path;
  double curtime;
  double robot_pos, cmd_pos;
  double start, end;    // shawn: calculate waste time

  /* Get the current status */
  SvoRead(&servo_svo);
  // Copy the status of Rm
  // robot_pos = rm.getPos();
  robot_pos = servo_svo.Refpos;
  servo_svo.Curpos = robot_pos;
  // Get current time
  curtime = GetCurrentTime();
  servo_svo.Time = curtime;

  // ----------- should be noticed ---------------
  if (servo_svo.PathFlag == ON & servo_svo.NewPathFlag == ON) {
    if (servo_svo.PathtailFlag == OFF) { // 判断堆栈是否已经弹空
      ret = GetTrjBuff(&path);
      if (ret == 0) {
        servo_svo.Path = path;
        SetStartTime(curtime);
      } else {
        servo_svo.PathtailFlag = ON; // 堆栈已空
      }
      servo_svo.NewPathFlag = ON;
    }
  }

  // Set path data
  if (servo_svo.NewPathFlag == ON) {
    servo_svo.Path.Orig = servo_svo.Curpos;
    servo_svo.NewPathFlag = OFF;
  }

  // Set servo data
  // Note: 这里保存数据最好先判断伺服标志是否置位，否则有可能Setsvo函数无法取得线程，
  // 导致开始时无法传递伺服运动参数。但是这样做的话，无法在伺服未启动时读取夹具状态。
  if (servo_svo.ServoFlag == ON) {
    if (servo_svo.PathFlag == ON) {
      // 多项式轨迹插补
      CalcRefPath(GetCurrentTime(), &servo_svo.Path, &servo_svo.Refpos);
      cmd_pos = servo_svo.Refpos;
    }

    if (servo_svo.ForceFlag == ON) {
      // 读取压力值
      start = GetCurrentTime();
      ADSingleV20(0, 0, &adResult);    // 单次采集
      // ADContinuV20(0, 0, 512, 100000, adBuf);    // 连续采集，一个数据包为512个数据
      // end = GetCurrentTime();
      // for(i=0;i<512;i++) {
      //   adResult += adBuf[i];
      // }
      // adResult /= 512.0;
      servo_svo.Curforce = (double)adResult;

      // PID控制
      servo_svo.Refpos = PID_Ctrl(&pid, servo_svo.Curforce, servo_svo.Refforce);
      cmd_pos = (float)servo_svo.Refpos;
    }

    /* 发起运动指令 */
    if(servo_svo.Curforce > 2.7) {cmd_pos = 0;}    // 压力预警
    rm->setPos(cmd_pos);

    servo_svo.temp = end - start;    // shawn: calculate waste time

    /* 保存数据到公共数据库 */
    SvoWrite(&servo_svo);

    /* 将数据保存到队列，用于存档 */
    ExpDataSave(&servo_svo);
  }
}

/*
 * 开始伺服控制。清空命令堆栈，将当前轨迹放入堆栈。
 */
void SetSvo(SVO *data) {
  int ret;
  double time;
  extern double kp, ki, kd;

  initTrjBuff();  // 初始化轨迹命令队列

  if (data->PathFlag == ON) {
    ret = PutTrjBuff(&data->Path);
    printf("ret = %d\n", ret);
    if (ret == 1)
      printf("PathBufferPut Error\n");
    else {
      printf("Done with PutTrjBuff.\n");
      printf("Goal position is %f\n", data->Path.Goal);
    }
    printf("> OUT frequency < %f [HZ]\n", data->Path.Freq);
    printf("> OUT mode < %d\n", data->Path.Mode);
  }

  if (data->ForceFlag == ON) {
    PID_Arg_Init(&pid, kp, ki, kd, data->Refpos);    // 重置PID控制器
  }

  // 设置伺服标志
  data->ServoFlag = ON;
  data->NewPathFlag = ON;
  data->PathtailFlag = OFF;
  
  // 重置时间
  ResetTime();
  time = GetCurrentTime();
  SetStartTime(time);

  SvoWrite(data);    // 将数据同步到pSVO
}

