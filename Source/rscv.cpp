/***********************************
 * 文件名称：rscv.cpp
 * 功    能:
 ***********************************/

/* **************************************************
 * Note:
 * 1. y = -20*e^(-2*x)*(1+2*x)
 * **************************************************/

#include "../include/rscv.h"
#include "../include/common.h"

extern shm_interface shm_servo_inter;    // 线程结束标志; (main)

void *rscv (void *param) {
  // 声明OpenCV图片矩阵
  cv::Mat src, cny, img;

  rs2::pipeline pipe;          // 声明Realsense管道
  rs2::frameset frames;        // 创建一个rs2::frameset对象, 包含一组帧和访问他们的接口
  rs2::colorizer color_map;    // 声明彩色图
 
  //Create a configuration for configuring the pipeline with a non default profile
  // 配置数据流信息
	rs2::config cfg;
	cfg.enable_stream(RS2_STREAM_COLOR, 640, 480, RS2_FORMAT_BGR8, 30);

  // 启动设备的管道配置文件, 开始传送数据流
	rs2::pipeline_profile selection = pipe.start(cfg);

  while(shm_servo_inter.status_control != EXIT_C) {
    frames = pipe.wait_for_frames();    // 等待下一帧

    // Get each frame
    rs2::frame color_frame = frames.get_color_frame();  // 获取彩色图

    // 创建Opencv中的Mat类,并传入数据
    cv::Mat src(cv::Size(640, 480), CV_8UC3, (void*)color_frame.get_data(), 
        cv::Mat::AUTO_STEP);

    // 展示图片
    cv::imshow("Display color Image", src);


    /* ************************************************* *
     * 图像处理
     * ************************************************* */
    // Canny只接受单通道8位图像, 边缘检测前先将图片转化为灰度图
    cv::cvtColor(src, img, cv::COLOR_BGR2GRAY);         // 灰度图
    cv::GaussianBlur(img, img, cv::Size(5,5), 0, 0);    // 高斯滤波
    // Canny参数影响轮廓轮廓识别能力
    cv::Canny(img, img, 40, 100);
    cv::imshow("Canny", img);
    // cv::waitKey(0);


    /* 等待n ms;如果有键盘输入则退出循环; 否则n ms后跳过并执行下一个循环
     * 在退出的时候,所有数据的内存空间将会被自动释放 */
    if(cv::waitKey(10) >= 0) {
      cv::imwrite("./pic1.jpg", src);
      break;
    }
  }
  return ((void *)0);
}


