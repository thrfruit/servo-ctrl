/***********************************
 * 文件名称：rscv.cpp
 * 功    能:
 ***********************************/

/* **************************************************
 * Note:
 * 1. y = -20*(1-e^(-2*x)*(1+2*x))
 * 2. Period: 0.03ms
 * **************************************************/

#include "../include/rscv.h"
#include "../include/common.h"

extern shm_interface shm_servo_inter;    // 线程结束标志; (main)

void *rscv (void *param) {
  // 复制共享变量
  SVO rscv_svo;

  // 声明OpenCV图片矩阵
  cv::Mat src, cny, img;

  // 储存直线信息
  std::vector<cv::Vec4i> tline;
  double delta_x, delta_y, k, h;
  double h_max = 0;

  rs2::pipeline pipe;          // 声明Realsense管道
  rs2::frameset frames;        // 创建一个rs2::frameset对象, 包含一组帧和访问他们的接口
  rs2::colorizer color_map;    // 声明彩色图
 
  //Create a configuration for configuring the pipeline with a non default profile
  // 配置数据流信息
	rs2::config cfg;
	cfg.enable_stream(RS2_STREAM_COLOR, 640, 480, RS2_FORMAT_BGR8, 30);

  // 启动设备的管道配置文件, 开始传送数据流
	rs2::pipeline_profile selection = pipe.start(cfg);

  while(true) {
    SvoRead(&rscv_svo);
    frames = pipe.wait_for_frames();    // 等待下一帧

    // Get each frame
    rs2::frame color_frame = frames.get_color_frame();  // 获取彩色图

    // 创建Opencv中的Mat类,并传入数据
    cv::Mat src(cv::Size(640, 480), CV_8UC3, (void*)color_frame.get_data(), 
        cv::Mat::AUTO_STEP);


    /* ************************************************* *
     * 图像处理
     * ************************************************* */
    // Canny只接受单通道8位图像, 边缘检测前先将图片转化为灰度图
    cv::cvtColor(src, img, cv::COLOR_BGR2GRAY);         // 灰度图
    cv::GaussianBlur(img, img, cv::Size(5,5), 0, 0);    // 高斯滤波
    // Canny参数影响轮廓轮廓识别能力
    cv::Canny(img, img, 40, 90);

    // 霍夫变换提取直线
    HoughLinesP(img, tline,1,CV_PI/90,14,6,16);

    for (size_t i=0; i<tline.size(); i++) {
      /* *** 计算直线斜率和高度 *** */
      delta_x = tline[i][2] - tline[i][0];
      delta_y = tline[i][3] - tline[i][1];
      if (delta_x == 0) {
        continue;
      }
      else{
        k = (double)(delta_y / delta_x);
        h = (double)((tline[i][1]+tline[i][3])/2*px2mm);
      }

      // 过滤得到水平直线
      if (k < 0.1) {
        cv::line(src, cv::Point(tline[i][0],tline[i][1]), 
          cv::Point(tline[i][2],tline[i][3]), 
          cv::Scalar(0, 0, 255),2,8);
        // 计算当前工具位置
        if (h - h_max > 0) {
          h_max = h;
        }
      }
      else { continue; }
    }  // for


    /* 同步共享数据 */
    rscv_svo.temp = h_max;
    SvoWrite(&rscv_svo);

    if(shm_servo_inter.status_control == EXIT_C) {
      cv::imwrite("./pic1.jpg", src);
      break;
    }  // if
  }  // while
  return ((void *)0);
}  // *rscv


