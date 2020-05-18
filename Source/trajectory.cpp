/***********************************
 * 文件名称：trajectory.cpp
 * 头 文 件：trajectory.h
 * 功    能：轨迹插补; 
 *           管理位移命令队列;
 ***********************************
 * TODO:
 * *********************************/

#include "../include/trajectory.h"

TRJ_BUFF PathBuff;

void initTrjBuff() { 
  PathBuff.data_num = 0; 
}

int PutTrjBuff(PATH *path) {
  int i, data_num;
  data_num = PathBuff.data_num;
  if (data_num >= TRJ_LENGTH) {
    std::cout << "ERROR: The path buffer is full !!!" << std::endl;
    return (1);
  } else {
    if (data_num > 0) {
      for (i = data_num; i > 0; i--) {
        PathBuff.Path[i] = PathBuff.Path[i - 1];
      }
    }
    PathBuff.Path[0] = *path;
    PathBuff.data_num = data_num + 1;
  }
  return 0;
}

int GetTrjBuff(PATH *path) {
  int i, data_num;

  data_num = PathBuff.data_num;

  if (data_num < 1) {
    std::cout << "ERROR: The path buffer is empty !!!" << std::endl;
    return 1;
  } else {
    *path = PathBuff.Path[0];
    for (i = 0; i < data_num; i++) {
      PathBuff.Path[i] = PathBuff.Path[i + 1];
    }
    PathBuff.data_num = data_num - 1;
  }
  return 0;
}

// 一次函数的轨迹插值
double Calc1JiTraje(double orig, double goal, double freq, double time) {
  double ref = goal;
  double time_n = freq * time;

  if (time_n <= 1)
    ref = orig + (goal - orig) * time_n;
  return ref;
}

// Sin函数的轨迹插值
double CalcSinTraje(double freq, double time) {
  double ref;
  ref = 6 + 6 * sin(freq * time);
  return ref;
}

void CalcRefPath(double curtime, PATH *path, double *pos) {
  double *orig, *goal, *ref;

  orig = &path->Orig;
  goal = &path->Goal;
  ref = pos;

  // 设置伺服间隔内运动的距离
  switch (path->Mode) {
  case PATH_1JI:
    *ref = Calc1JiTraje(*orig, *goal, path->Freq, curtime);
    break;
  case PATH_SIN:
    *ref = CalcSinTraje(path->Freq, curtime);
    break;
  default:
    *ref = Calc1JiTraje(*orig, *goal, path->Freq, curtime);
  }
}

