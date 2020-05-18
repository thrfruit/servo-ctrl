
/* 
 * servo.h
 */

#ifndef SERVO_H
#define SERVO_H

#include"common.h"
#include"rmctrl.h"
extern double omn_df;

void servo_function(RmDriver *rm);
void servo_function_test();
void CalcRefPath(double curtime, PATH *path, double *pos, double *dpos);

#endif

