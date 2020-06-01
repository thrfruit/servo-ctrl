/***********************************
 * 文件名称：rscv.h
 * 源 文 件：rscv.cpp
 * 功    能: 数据采集线程
 *           相机数据采集, 图像处理,
 *           计算期望压力等
 ***********************************/

#ifndef RSCV_H
#define RSCV_H

// Realsense
#include <librealsense2/rs.hpp>
#include<librealsense2/rsutil.h>
#include <librealsense2/hpp/rs_processing.hpp>
#include <librealsense2/hpp/rs_types.hpp>
#include <librealsense2/hpp/rs_sensor.hpp>

// OpenCV
// 会减慢编译速度, 推荐逐个添加必须的库
// #include <opencv2/opencv.hpp>        // 包含OpenCV各个模块的头文件

#include <opencv2/highgui/highgui.hpp>    // 显示窗口,滑动条,鼠标操作以输入输出相关
#include <opencv2/imgproc/imgproc.hpp>    // 图像处理函数
#include <opencv2/core/core.hpp>    // 新式C++风格的结构以及数学运算

/* Extral headers */
#include "dataexchange.h"
#include "system.h"
#include <math.h>


#define px2mm  0.153846
#define gravity 9.8


void *rscv (void *param);

#endif

