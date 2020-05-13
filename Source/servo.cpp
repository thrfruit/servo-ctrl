/*
 * servo.cpp
 * 伺服进程的
 */

#include "../include/servo.h"
#include "../include/common.h"
#include "../include/interface.h"
#include "../include/motion_axis.h"
#include "../include/system.h"
#include "../include/trajectory.h"
#include"../include/WzSerialPort.h"
#include"../include/usb-daq-v20.h"
#include <errno.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <vector>

int cnt = 0;
// 串口通信
WzSerialPort serial;
char buf[1024];
float adBuf[1024];
// PID控制器
double omn_df;
double kp = -0.1;
double ki = -0.01;

void servo_function() {
  int ret;
  int i;
  double curtime;
  double cmd_pos;
  double uv;
  double df;
  SVO servo_svo;
  PATH path;

  double robot_pos;
  double robot_dpos;
  rm_axis_handle handle = 0;
  clock_t start, end;
  float adResult = 0.0;

  start = clock();

  SvoReadFromGui(&servo_svo);

  // Get the current status of robot
  // robot_pos = rm_read_current_position(handle);
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
      CalcRefPath(GetCurrentTime(), &servo_svo.Path, &servo_svo.Refpos,
                  &servo_svo.Refdpos);
      cmd_pos = servo_svo.Refpos;
      // cmd_pos = servo_svo.Time;
    }

    if (servo_svo.ForceFlag == ON) {
      // 读取压力值
      // ADSingleV20(0, 0, &adResult);
      ADContinuV20(0, 0, 512, 100000, adBuf);
      for(i=0;i<50;i++) {
        adResult += adBuf[i];
      }
      adResult /= 50.0;
      servo_svo.Curforce = (double)adResult;

      // PID控制
      df = servo_svo.Curforce - servo_svo.Refforce;
      omn_df += df;
      uv = kp*df + ki*omn_df;
      servo_svo.Refpos = servo_svo.Curpos + uv;
      // if(servo_svo.Refpos <= 0) {
      //   servo_svo.Refpos = 0;
      //   omn_df = 0;
      // }
      // else if (servo_svo.Refpos >= 10) {
      //   servo_svo.Refpos = 10;
      //   omn_df = 0;
      // }
      cmd_pos = servo_svo.Refpos;
    }

    /* 发起运动指令
     * rm_move_absolute: 运动到绝对位置；
     * rm_push: 以规定力推动到相对位置；*/
    // cmd_pos = servo_svo.Refpos - servo_svo.Curpos;
    // rm_push(handle, 20, cmd_pos, 10);
    // rm_move_absolute(handle, cmd_pos, 200, 3000, 3000, 0.1);
    rm_set_position(handle, cmd_pos);

    end = clock();

    servo_svo.temp = (double)(end-start)/CLOCKS_PER_SEC;

    /* 保存数据到公共数据库 */
    SvoWrite(&servo_svo);

    /* 将数据保存到队列，用于存档 */
    ExpDataSave(&servo_svo);
  }

  cnt++;
}

