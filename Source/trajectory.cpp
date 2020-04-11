/* 伺服命令的堆栈处理
 *
 */

#include"../include/trajectory.h"
#define TRJ_LENGTH 20

typedef struct
{
        int data_num;
        PATH Path[TRJ_LENGTH];
}TRJ_BUFF;

TRJ_BUFF PathBuff;

void initTrjBuff()
{
        PathBuff.data_num = 0;
}

int PutTrjBuff(PATH *path) {
  int i, data_num;
  data_num = PathBuff.data_num;
  if (data_num >= TRJ_LENGTH) {
    printf("Error: The path buffer is full!!!\n");
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

int GetTrjBuff(PATH *path)
{
        int i, data_num;

        data_num = PathBuff.data_num;

        if(data_num <1){
                printf("Error: The path buffer is empty!!!\n");
                return 1;
        }
        else{
                *path = PathBuff.Path[0];
                for(i=0; i<data_num; i++){
                        PathBuff.Path[i] = PathBuff.Path[i+1];
                }
                PathBuff.data_num = data_num - 1;
        }
        return 0;
}


// 一次函数的轨迹插值
double Calc1JiTraje(double orig, double goal, double freq, double time)
{
        double ref = goal;
        double time_n = freq*time;

        if(time_n <= 1)
                ref = orig + (goal-orig)*time_n;
        return ref;
}

