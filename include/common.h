
#ifndef COMMON_H
#define COMMON_H
extern "C"
{

#include <stdio.h>//标准输入输出 “standard input & output"（标准输入输出）
#include <stdlib.h>//standard library标准库函数 头文件
#include <string.h>//字符数组的函数定义的头文件，strlen、strcmp、strcpy等等
#include <unistd.h>//对 POSIX 操作系统 API 的访问功能的头文件，操作系统如 Unix 的所有官方版本，包括 Mac OS X、Linux 等
#include <math.h>
#include <time.h>//日期和时间头文件。用于需要时间方面的函数。
#include <pthread.h>//实现 POSIX 线程标准的库常被称作Pthreads，一般用于Unix-likePOSIX 系统如Linux、Solaris
// 可参考：Linux多线程编程实例解析(https://blog.csdn.net/zqixiao_09/article/details/50298693)
#include <unistd.h>//和上面重复了。。。
#include <sys/types.h>//是Unix/Linux系统的基本系统数据类型的头文件，含有size_t，time_t，pid_t等类型。
#include <sys/mman.h>//将一个文件或者其它对象映射进内存，<sys/mman.h>是Unix头文件
/*Eigen header*/
// #include <eigen3/Eigen/Dense>

#define DEBUG
#define ON 1
#define OFF 0
#define INIT_C 0
#define EXIT_C 255
#define EXP_DATA_LENGTH 10000
#define EXP_DATA_INTERVAL 1
#define NSEC_PER_SEC	(1000000000)
#define Rad2Deg 180.0/M_PI
#define Deg2Rad M_PI/180.0

struct shm_interface{
  int status_print;
  int status_control;
};

// Global
typedef struct{
  double Orig;
  double Goal;
  double Freq;
  int Mode;
}PATH;

typedef struct{
  int ServoFlag;
  int PathFlag;
  int ForceFlag;
	int NewPathFlag;
	int PathtailFlag;
  double Refforce;
  double Curforce;
	double Time;
  PATH Path;
	double Curpos;
	double Refpos;
	double Refdpos;
  double pos;
	double dpos;
  double temp;
}SVO;

}
#endif

