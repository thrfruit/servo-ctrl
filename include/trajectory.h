
#ifndef TRAJECTORY_H
#define TRAJECTORY_H

#include"./common.h"

/* trajectory.cpp */
extern void initTrjBuff();
extern int PutTrjBuff(PATH* path);
extern int GetTrjBuff(PATH* path);
extern double Calc1JiTraje(double orig, double goal, double freq, double time);
extern double CalcSinTraje(double time);

#endif

