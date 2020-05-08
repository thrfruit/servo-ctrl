
#ifndef INTERFACE_H
#define INTERFACE_H

#include"./common.h"

/* dataExchange.cpp */
extern void SvoWriteFromServo(SVO *data);
extern void SvoReadFromGui(SVO *data);
extern void SvoReadFromDis(SVO *data);
extern void ChangePosData(PATH *Path);
extern void SetSvo(SVO *data);
extern void SvoWrite(SVO *data);
extern void SvoRead(SVO *data);

/* dataSave.cpp */
extern void ExpDataSave(SVO *data);
extern void SaveDataReset();
extern void ExpDataWrite();
#endif

