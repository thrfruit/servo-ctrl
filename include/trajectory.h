
#ifndef TRAJECTORY_H
#define TRAJECTORY_H

#include"./common.h"

/* trajectory.cpp */
void initTrjBuff();
int PutTrjBuff(PATH* path);
int GetTrjBuff(PATH* path);
double Calc1JiTraje(double orig, double goal, double freq, double time);
double CalcSinTraje(double time);

#endif

