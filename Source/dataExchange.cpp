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
    file1 << std::left << setw(len) << "Time"
          << setw(len) <<"Cur_h"
          << setw(len) <<"Ref_h"
          << endl;
    file2 << std::left << setw(len) << "Time"
          << setw(len) <<"Curforce" 
          << setw(len) <<"Refforce"
          << setw(len) <<"Ref_pos"
          << setw(len) <<"Time_rscv"
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
            << setw(len) << Exp_data[i].Time.Curtime
            << setw(len) << Exp_data[i].Motion.Curh 
            << setw(len) <<Exp_data[i].Motion.Refh
            << endl;
      // Force
      file2 << std::left
            << setw(len) << Exp_data[i].Time.Curtime
            << setw(len) << Exp_data[i].State.Curforce
            << setw(len) << Exp_data[i].State.Refforce
            << setw(len) << Exp_data[i].State.Refpos
            << setw(len) << Exp_data[i].Time.Rscv_time
            << endl;
      // Adaptation
      file3 << std::left 
            << setw(len) << Exp_data[i].Adapt.hr
            << setw(len) << Exp_data[i].Adapt.s
            << setw(len) << Exp_data[i].Adapt.dh
            << setw(len) << Exp_data[i].Adapt.a_hat
            << setw(len) << Exp_data[i].Adapt.b_hat
            << setw(len) << Exp_data[i].Adapt.c_hat
            << setw(len) << Exp_data[i].State.Refforce
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

