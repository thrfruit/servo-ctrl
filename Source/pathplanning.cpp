/* Path planning
 */

#include"../include/servo.h"
#include"../include/trajectory.h"

#define PATH_SIN 0
#define PATH_5JI 3
#define PATH_3JI 2
#define PATH_1JI 1

using namespace std;

void CalcRefPath(double curtime, PATH *path, double *pos, double *dpos) {
  double *orig, *goal, *ref, *ref_v;

  orig = &path->Orig;
  goal = &path->Goal;
  ref = pos;
  ref_v = dpos;

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

  // 设置私服间隔内运动的速度
  switch (path->Mode) {
  default:
    *ref_v = 0.;
  }
}
