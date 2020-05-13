
#ifndef INTERFACE_H
#define INTERFACE_H

#include"./common.h"

/* dataExchange.cpp */
void SvoWriteFromServo(SVO *data);
void SvoReadFromGui(SVO *data);
void SvoReadFromDis(SVO *data);
void ChangePosData(PATH *Path);
void SetSvo(SVO *data);
void SvoWrite(SVO *data);
void SvoRead(SVO *data);

/* dataSave.cpp */
void ExpDataSave(SVO *data);
void SaveDataReset();
void ExpDataWrite();
#endif

