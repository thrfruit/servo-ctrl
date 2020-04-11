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
#include <errno.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <vector>

int cnt = 0;

void servo_function() {
  int ret;
  double curtime;
  double cmd_pos;
  SVO servo_svo;
  PATH path;

  double robot_pos;
  double robot_dpos;
  int handle = 0;

  // Get current time
  curtime = GetCurrentTime();
  SvoReadFromGui(&servo_svo);
  servo_svo.Time = curtime;

  // Get the current status of robot
  robot_pos = rm_read_current_position(handle);

  // Cooy the status of robot
  servo_svo.Curpos = robot_pos;

  // ----------- should be noticed ---------------
  if (servo_svo.NewPathFlag == ON) {
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
  // 导致开始时无法传递伺服运动参数。
  if (servo_svo.ServoFlag == ON) {
    CalcRefPath(GetCurrentTime(), &servo_svo.Path, &servo_svo.Refpos,
                &servo_svo.Refdpos);
    /* 发起运动指令 */
    cmd_pos = servo_svo.Refpos;
    /* 保存数据到公共数据库 */
    SvoWrite(&servo_svo);
  }

  rm_move_absolute(handle, cmd_pos, 200, 3000, 3000, 0.1);

  cnt++;
}

