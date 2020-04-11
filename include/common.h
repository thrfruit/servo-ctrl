
#ifndef COMMON_H
#define COMMON_H
extern "C"
{

#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>

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
	int NewPathFlag;
	int PathtailFlag;
	double Time;
  PATH Path;
	double Curpos;
	double Refpos;
	double Refdpos;
    double pos;
	double dpos;
}SVO;

}
#endif

