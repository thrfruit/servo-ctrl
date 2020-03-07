
#ifndef SYSTEM_H
#define SYSTEM_H

#include"./common.h"

extern double GetCurrentTime();
extern void ResetTime();
extern void ResetTimerCounter();
extern void TimeCounterAdd();
extern int GetTimerCounter();
extern double GetOffsetTime();
extern void SetStartTime(double time);

#endif

