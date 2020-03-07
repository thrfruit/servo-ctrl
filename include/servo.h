
/* 
 * servo.h
 */

#ifndef SERVO_H
#define SERVO_H

#include"../include/common.h"

extern void servo_function();
extern void CalcRefPath(double curtime, PATH *path, double *pos, double *dpos);

#endif

