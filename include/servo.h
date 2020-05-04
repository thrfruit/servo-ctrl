
/* 
 * servo.h
 */

#ifndef SERVO_H
#define SERVO_H

#include"../include/common.h"
extern double omn_df;

extern void servo_function();
extern void servo_function_test();
extern void CalcRefPath(double curtime, PATH *path, double *pos, double *dpos);

#endif

