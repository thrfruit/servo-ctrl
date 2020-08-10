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
#include "../include/servo.h"

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
// for saving the data
int Exp_data_index = 0;
int Servo_data_index = 0;
int Rscv_data_index = 0;
SVO Exp_data[EXP_DATA_LENGTH];
SERVO Servo_data[EXP_DATA_LENGTH];
RSCV Rscv_data[EXP_DATA_LENGTH];

/* *** SVO Data *** */
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
  std::ofstream file1, file2, file3;

  // 此处路径从运行路径填起
  file1.open("./data/data.position");

  if(file1.is_open()) {
    file1 << std::left << std::setw(len) << "Time"
          << std::setw(len) <<"Cur_h"
          << std::setw(len) <<"Ref_h"
          << std::endl;
    for(i=0; i<Exp_data_index; i++) {
      file1 << std::left 
            << std::setw(len) << Exp_data[i].Time.Curtime
            << std::setw(len) << Exp_data[i].Motion.Curh 
            << std::setw(len) << Exp_data[i].Motion.Refh
            << std::endl;
    }
    file1.close();
  } 
  else {
    printf("open file failed\n");
  }
}

/* *** Servo Data *** */
void ServoSaveDataReset() {
  Servo_data_index = 0;
}

void ServoDataSave(SERVO *data) {
  if(Servo_data_index < EXP_DATA_LENGTH) {
    Servo_data[Servo_data_index] = *data;
    Servo_data_index++;
  }
}

void ServoDataWrite() {
  int i, len=16;
  std::ofstream file_servo;

  // 此处路径从运行路径填起
  file_servo.open("./data/data.force");

  if(file_servo.is_open()) {
    file_servo << std::left
      << std::setw(len) << "Time"
      << std::setw(len) << "refforce"
      << std::setw(len) << "curforce"
      << std::setw(len) << "cmd_pos"
      << std::endl;
    for(i=0; i<Servo_data_index; i++) {
      file_servo << std::left 
        << std::setw(len) << Servo_data[i].time
        << std::setw(len) << Servo_data[i].refforce
        << std::setw(len) << Servo_data[i].curforce
        << std::setw(len) << Servo_data[i].curpos
        << std::endl;
    }
    file_servo.close();
  }
  else {
    std::cout << "open file failed" << std::endl;
  }
}

/* *** Rscv Data *** */
void RscvSaveDataReset() {
  Rscv_data_index = 0;
}

void RscvDataSave(RSCV *data) {
  if(Rscv_data_index < EXP_DATA_LENGTH) {
    Rscv_data[Rscv_data_index] = *data;
    Rscv_data_index++;
  }
}

void RscvDataWrite() {
  int i, len=16;
  std::ofstream file_rscv;

  // 此处路径从运行路径填起
  file_rscv.open("./data/data.adaptation");

  if(file_rscv.is_open()) {
    file_rscv << std::left
      << std::setw(len) << "Time_rscv"
      << std::setw(len) << "s"
      << std::setw(len) << "dh"
      << std::setw(len) << "hr"
      << std::setw(len) << "a_hat"
      << std::setw(len) << "ufn"
      << std::endl;
    for(i=0; i<Rscv_data_index; i++) {
      file_rscv << std::left 
        << std::setw(len) << Rscv_data[i].time
        << std::setw(len) << Rscv_data[i].s
        << std::setw(len) << Rscv_data[i].dh
        << std::setw(len) << Rscv_data[i].hr
        << std::setw(len) << Rscv_data[i].a_hat
        << std::setw(len) << Rscv_data[i].ufn
        << std::setw(len) << Rscv_data[i].cost_time
        << std::endl;
    }
    file_rscv.close();
  }
  else {
    std::cout << "! ! ! Rscv: open file failed" << std::endl;
  }
}

