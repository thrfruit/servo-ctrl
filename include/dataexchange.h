/***********************************
 * 文件名称：dataExchange.h
 * 源 文 件：dataExchange.cpp
 * 功    能：处理线程之间的数据交换;
 *           文件读写;
 ***********************************/

#ifndef DATAEXCHANGE_H
#define DATAEXCHANGE_H


#include "./common.h"
/* 文件读写 */
#include <fstream>
#include <iostream>
#include <iomanip>

/* ================ dataExchange.cpp ================ */
/***********************************
 * 函数名称：SvoWrite
 * 功    能：将当前线程的共享数据 *_svo 写入全局变量 pSVO
 * 参    数：SVO 当前线程的共享数据结构体 *_svo
 * 返 回 值：void
 ***********************************/
void SvoWrite(SVO *data);

/***********************************
 * 函数名称：SvoRead
 * 功    能：读取全局的共享数据 pSVO 到当前线程 *_svo
 * 参    数：SVO 待写入的共享数据结构体 *_svo
 * 返 回 值：void
 ***********************************/
void SvoRead(SVO *data);


/* ================ dataSave.cpp ================ */
#define EXP_DATA_LENGTH 10000    // 最大的存档数据长度
#define EXP_DATA_INTERVAL 1      // 存档数据的保存间隔

/***********************************
 * 函数名称：ExpDataSave
 * 功    能：将一个 SVO 结构体加入待存档队列
 * 参    数：SVO 待存档的共享数据结构体
 * 返 回 值：void
 ***********************************/
void ExpDataSave(SVO *data);

/***********************************
 * 函数名称：SaveDataReset
 * 功    能：重置存档数据的队列
 * 参    数：NULL
 * 返 回 值：void
 ***********************************/
void SaveDataReset();

/***********************************
 * 函数名称：ExpDataWrite
 * 功    能：将待存档数据写入文档
 * 参    数：NULL
 * 返 回 值：void
 ***********************************/
void ExpDataWrite();

#endif

