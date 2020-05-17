
#ifndef SYSTEM_H
#define SYSTEM_H

#include"./common.h"

double GetCurrentTime();
void ResetTime();
void ResetTimerCounter();
void TimeCounterAdd();
int GetTimerCounter();
double GetOffsetTime();
void SetStartTime(double time);

#endif

