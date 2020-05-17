
/* 
 * servo.h
 */

#ifndef SERVO_H
#define SERVO_H

#include"../include/common.h"
extern double omn_df;

void servo_function();
void servo_function_test();
void CalcRefPath(double curtime, PATH *path, double *pos, double *dpos);

#endif

