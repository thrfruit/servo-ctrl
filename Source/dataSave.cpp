
#include <fstream>
#include <iostream>
#include <iomanip>
#include "../include/common.h"

#define EXP_DATA_LENGTH 10000
#define EXP_DATA_INTERVAL 1

using namespace std;

int flag_WriteData = OFF;
int flag_SaveData = OFF;

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
  ofstream file1, file2;
  int i;

  // 此处路径从Cmake根目录填起
  file1.open("data/data.position");
  file2.open("data/data.force");

  if(file1.is_open() & file2.is_open()) {
    printf("Saving data ...\n");
    file1 << std::left << setw(12) << "Time" << setw(12) <<"Curpos" 
          << setw(12) <<"Refpos" << endl;
    file2 << std::left << setw(12) << "Time" << setw(12) <<"Curforce" 
          << setw(12) <<"Refforce" << endl;

    for(i=0; i<Exp_data_index; i++) {
      file1 << std::left << setw(12) << Exp_data[i].Time 
            << setw(12) << Exp_data[i].Curpos 
            << setw(12) <<Exp_data[i].Refpos 
            << setw(12) <<Exp_data[i].temp
            << endl;
      file2 << std::left << setw(12) << Exp_data[i].Time
            << setw(12) << Exp_data[i].Curforce
            << setw(12) << Exp_data[i].Refforce
            << setw(12) <<Exp_data[i].temp
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

