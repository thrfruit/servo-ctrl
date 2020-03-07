/* servo.cpp
 * 
 */

#include<stdio.h>
#include<stdlib.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<string.h>
#include<errno.h>
#include<vector>
#include<iostream>
#include"../include/trajectory.h"
#include"../include/servo.h"
#include"../include/interface.h"
#include"../include/system.h"

int cnt = 0;
extern void* shm_addr;
using namespace std;
extern SVO pSVO;
extern pthread_mutex_t servoMutex;

void servo_function()
{
        int ret;
        double curtime;
        double com_pos;
        SVO servoP;
        PATH path;

        double robot_pos;
        double robot_dpos;
        
        // Get current time
        curtime = GetCurrentTime();
        SvoReadFromGui(&servoP);
        servoP.Time = curtime;

        // Get the current status of robot
        robot_pos = servoP.pos;

        // Cooy the status of robot
        servoP.Curpos = robot_pos;
        

        // ----------- should be noticed ---------------
        if(servoP.NewPathFlag == ON){
                if(servoP.PathtailFlag == OFF){
                        ret = GetTrjBuff(&path);
                        if(ret == 0){
                                servoP.Path = path;
                                SetStartTime(curtime);
                        }
                        else{
                                servoP.PathtailFlag = ON;
                        }
                        servoP.NewPathFlag = ON;
                }
        }

        // Set path data
        if(servoP.NewPathFlag == ON){
                servoP.Path.Orig = servoP.Curpos;
                servoP.NewPathFlag = OFF;
        }

        // Set servo data
        if(servoP.ServoFlag == ON){
                CalcRefPath(GetCurrentTime(), &servoP.Path, &servoP.Refpos, &servoP.Refdpos);
        }

        /* 发起运动指令 */
        com_pos = servoP.Refpos;
        servoP.Curpos = com_pos;
        pthread_mutex_lock(&servoMutex);
        pSVO.Curpos = com_pos;
        pSVO.NewPathFlag = servoP.NewPathFlag;
        pthread_mutex_unlock(&servoMutex);
}
        


