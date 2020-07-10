/***********************************
 * 文件名称：dataExchange.cpp
 * 头 文 件：dataExchange.h
 * 功    能：处理线程之间的数据交换;
 *           文件读写;
 ***********************************
 * TODO:
 * *********************************/

#include "../include/common.h"
#include "../include/dataexchange.h"

/* ============== 线程之间的数据传递 ================ */
// Shared variable
SVO pSVO;
pthread_mutex_t servoMutex = PTHREAD_MUTEX_INITIALIZER;

void SvoWrite(SVO *data) {
  pthread_mutex_lock(&servoMutex);
  pSVO = *data;
  pthread_mutex_unlock(&servoMutex);
}

void SvoRead(SVO *data) {
  pthread_mutex_lock(&servoMutex);
  *data = pSVO;
  pthread_mutex_unlock(&servoMutex);
}

/*
 * 从键盘读取运动参数到当前线程
 */
void ChangePosData(PATH *Path) {
  printf("Path frequency [1/s] = \n");
  scanf("%lf", &Path->Freq);

  printf("Path mode: Sin(0) 1JI(1)\n");
  printf("Path mode = \n");
  scanf("%d", &Path->Mode);

  printf("Position of claw (mm) = \n");
  scanf("%lf", &Path->Goal);
}


/* ============== 文件处理 ================ */
using namespace std;

// for saving the data
int Exp_data_index = 0;
SVO Exp_data[EXP_DATA_LENGTH];


void ExpDataSave(SVO *data) {
  if(Exp_data_index < EXP_DATA_LENGTH) {
    Exp_data[Exp_data_index] = *data;
    Exp_data_index++;
  }
}

void SaveDataReset() {
  Exp_data_index = 0;
}


void ExpDataWrite() {
  int i, len=16;
  ofstream file1, file2, file3;

  // 此处路径从运行路径填起
  file1.open("./data/data.position");
  file2.open("./data/data.force");
  file3.open("./data/data.adaptation");

  if(file1.is_open() & file2.is_open()) {
    printf("Saving data ...\n");
    file1 << std::left << setw(len) << "Time_rscv"
          << setw(len) <<"Cur_h"
          << setw(len) <<"Ref_h"
          << endl;
    file2 << std::left << setw(len) << "Time"
          << setw(len) <<"Curforce" 
          << setw(len) <<"Refforce"
          << setw(len) <<"Ref_pos"
          << endl;
    file3 << std::left 
          << setw(len) << "hr"
          << setw(len) << "s"
          << setw(len) << "dh"
          << setw(len) << "a_hat"
          << setw(len) << "b_hat"
          << setw(len) << "c_hat"
          << setw(len) << "Refforce"
          << endl;

    for(i=0; i<Exp_data_index; i++) {
      // Position
      file1 << std::left 
            << setw(len) << Exp_data[i].Time
            << setw(len) << Exp_data[i].Motion.Curh 
            << setw(len) <<Exp_data[i].Motion.Refh
            << setw(len) <<Exp_data[i].temp
            << endl;
      // Force
      file2 << std::left << setw(len) << Exp_data[i].Time
            << setw(len) << Exp_data[i].Curforce
            << setw(len) << Exp_data[i].Refforce
            << setw(len) << Exp_data[i].Refpos
            << setw(len) << Exp_data[i].temp
            << endl;
      // Adaptation
      file3 << std::left 
            << setw(len) << Exp_data[i].hr
            << setw(len) << Exp_data[i].s
            << setw(len) << Exp_data[i].dh
            << setw(len) << Exp_data[i].a_hat
            << setw(len) << Exp_data[i].b_hat
            << setw(len) << Exp_data[i].c_hat
            << setw(len) << Exp_data[i].Refforce
            << endl;
    }

    file1.close();
    file2.close();
    printf("Data saved.\n");
  } 
  else {
    printf("open file failed\n");
  }
}

