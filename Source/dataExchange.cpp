/* 不同线程和函数之间的数据交换
 *
 */

#include"../include/common.h"
#include"../include/trajectory.h"
#include"../include/system.h"

pthread_mutex_t servoMutex = PTHREAD_MUTEX_INITIALIZER;

// Shared variable
SVO pSVO;

void SvoWriteFromServo(SVO *data)
{
    pthread_mutex_lock(&servoMutex);
    pSVO=*data;
    pthread_mutex_unlock(&servoMutex);
}

void SvoReadFromGui(SVO *data)
{
        pthread_mutex_lock(&servoMutex);
        *data = pSVO;
        pthread_mutex_unlock(&servoMutex);
}

void SvoReadFromDis(SVO *data)
{
        pthread_mutex_lock(&servoMutex);
        *data = pSVO;
        pthread_mutex_unlock(&servoMutex);
}

void ChangePosData(PATH *Path)
{
        printf("Path frequency [1/s] = \n");
        scanf("%lf", &Path->Freq);
        printf("Path mode: 1JI(1)\n");
        printf("Path mode = \n");
        scanf("%d", &Path->Mode);

        printf("Position of claw (mm) = \n");
        scanf("%lf", &Path->Goal);
}

/* 设置伺服命令：伺服命令堆栈处理；
 */
void SetSvo(SVO *data)
{
        int ret;
        double time;

        pSVO.ServoFlag = data->ServoFlag;
        pSVO.Path = data->Path;

        initTrjBuff();
        ret = PutTrjBuff(&pSVO.Path);
        printf("ret = %d\n",ret);
        if(ret == 1)
                printf("PathBufferPut Error\n");
        else{
                printf("Down with PutTrjBuff.\n");
                printf("Goal position is %f\n", pSVO.Path.Goal);
        }
        printf("> OUT frequency < %f [HZ]\n", pSVO.Path.Freq);
        printf("> OUT mode < %d\n", pSVO.Path.Mode);

        pSVO.ServoFlag = ON;
        pSVO.NewPathFlag = ON;
        pSVO.PathtailFlag = OFF;

        ResetTime();
        time=GetCurrentTime();
        SetStartTime(time);
}

