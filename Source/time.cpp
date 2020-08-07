/***********************************
 * 文件名称：time.cpp
 * 头 文 件：system.h
 * 功    能：时间处理的函数;
 ***********************************
 * TODO:
 * *********************************/

#include "../include/system.h"

unsigned long long int tsc0;
static double StartTime = 0.0;

double GetCurrentTime() {
  struct timespec time;
  double curTime;
  unsigned long long int tsc1;
  clock_gettime(CLOCK_REALTIME, &time);
  tsc1 = (long long int)(time.tv_sec) * NSEC_PER_SEC +
         (long long int)(time.tv_nsec);

  curTime = (long double)(tsc1 - tsc0) / NSEC_PER_SEC;
  return curTime;
}

void ResetTime() {
  unsigned long long int tsc1;
  struct timespec time;
  clock_gettime(CLOCK_REALTIME, &time);
  tsc1 = (long long int)(time.tv_sec) * NSEC_PER_SEC +
         (long long int)(time.tv_nsec);
  tsc0 = tsc1;
}

double GetOffsetTime(void) { 
  return GetCurrentTime() - StartTime; 
}

void SetStartTime(double time) {
  StartTime = time;
  // printf("Set start time, t=%f\n", time);
}

