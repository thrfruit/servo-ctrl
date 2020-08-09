/*********************************** 
 * 文件名称：rscv.cpp
 * 功    能:
 * 思    路: 
 * 获取图片 -> 提取轮廓 -> 霍夫变换提取直线
 * -> 计算工具位置 -> 更新状态变量 -> 更新自适应控制参数
 * -> 计算输出信号
 *  周   期: 约30~40ms
 ***********************************/
/***********************************
 * TODO:
 * 1. 如果某次没检测到工具, 该怎样处理; 
 *    认为工具静止 / 夹紧以便于检测
 * 2. 自适应控制系数积分饱和的问题;
 * *********************************/

#include "../include/rscv.h"
#include "../include/common.h"

extern shm_interface shm_servo_inter;    // 线程结束标志; (main)
extern pthread_mutex_t mymutex;       // Shanw互斥锁
extern pthread_cond_t rt_msg_cond;    // Shawn条件变量
extern double force_0;

SVO rscv_svo;
RSCV Rscv;

void *rscv (void *param) {
  // 储存直线信息
  double h_cur = 0, h_last = 0, h_orig, h_min;

  /* *** 参考模型的状态参数 *** */
  /* s: 跟踪误差(广义误差);    ks: 跟踪误差系数;
   * d2hr: 加速度误差;    dh: 速度误差;
   * *_hat: 自适应控制系数
   * aa, ab, ac: 调参系数
   * ufn: 输出压力信号
   */
  double hm, dhm, d2hm;
  double a_hat, b_hat, c_hat, ks=6;
  double aa = 2, ab = 200, ac = 200;
  double hr, s, dh;
  double time=0, time_last=0, dt;
  double lambda = 10;
  double ufn, exp_t;
  double start, end;
  double ret;

  // 声明Realsense管道
  rs2::pipeline pipe;
  // 创建一个rs2::frameset对象, 包含一组帧和访问他们的接口
  rs2::frameset frames;
  // 声明彩色图
  rs2::colorizer color_map;
  // 配置管道的数据流信息
  rs2::config cfg;
  cfg.enable_stream(RS2_STREAM_COLOR, 640, 480, RS2_FORMAT_BGR8, 30);
  // 启动设备的管道配置文件, 开始传送数据流
  rs2::pipeline_profile selection = pipe.start(cfg);

  RscvSaveDataReset();

  // 设置原点
  // 注意正负号的问题：相机获得的数据向下为正,公式计算中向上为正
  h_orig = -1*setOrig(frames, pipe)/1000;

  while(true) {
    /* *** 线程同步 *** */
    pthread_mutex_lock(&mymutex);
    pthread_cond_wait(&rt_msg_cond, &mymutex);
    pthread_mutex_unlock(&mymutex);

    /* *** Get the current status *** */
    SvoRead(&rscv_svo);
    time = rscv_svo.Time.Curtime;
    // 期望运动状态
    hm   = rscv_svo.Motion.Refh;
    dhm  = rscv_svo.Motion.dhm;
    d2hm = rscv_svo.Motion.d2hm;

    /* **************************************************
     * 计算工具运动状态
     * **************************************************/
    // 检测物体上边缘
    ret = getLine(frames, pipe);
    // 判断是否检测到物体
    if (ret +1 != 0){
      h_min = -1*ret/1000;
    }
    // 工具只能下落
    if (h_min - (h_cur+h_orig) > 0) {
      h_min = h_cur + h_orig;
    }

    // 更新时间信息
    dt        = time - time_last;
    time_last = time;

    // 运动状态更新
    h_last = h_cur;
    h_cur  = h_min - h_orig;
    // 处理除零错误
    if (dt != 0) {
      dh = (h_cur - h_last) / dt;
    }

    /* **************************************************
     * 模型参考自适应控制(MRAS)
     * **************************************************/
    // 计算广义误差(注意正负号的问题)
    hr     = d2hm - lambda*dh;
    s      = (dh - dhm) + lambda*(h_cur - hm);
    // 更新自适应控制参数
    a_hat += -aa*s*(hr+gravity)*dt;
    // 计算输出信号
    ufn    = force_0 + a_hat*(hr+gravity) - ks*s;

    /* *** 同步共享数据 *** */
    rscv_svo.Time.Rscv_time = time;
    rscv_svo.State.Refforce = ufn;
    rscv_svo.Motion.Curh    = h_cur;
    Rscv.time  = time;
    Rscv.curh  = h_cur;
    Rscv.hr    = hr;
    Rscv.s     = s;
    Rscv.dh    = dh;
    Rscv.a_hat = a_hat;
    Rscv.ufn   = ufn;
    SvoWrite(&rscv_svo);
    RscvDataSave(&Rscv);

    /*** 设置退出条件 ***/
    if(shm_servo_inter.status_control == EXIT_C) {
      break;
    }  // if
  }  // while
  RscvDataWrite();
  std::cout << "=== Rscv: data saved." << std::endl;
  return ((void *)0);
}  // *rscv


double setOrig(rs2::frameset frames, rs2::pipeline pipe) {
  cv::Mat img;
  // 储存直线信息
  std::vector<cv::Vec4i> tline;
  double delta_x, delta_y, k, h;
  double h_min;
  int id, flag;

  while(true) {
    frames = pipe.wait_for_frames();    // 等待下一帧

    // Get each frame
    rs2::frame color_frame = frames.get_color_frame();  // 获取彩色图

    // 创建Opencv中的Mat类,并传入数据
    cv::Mat src(cv::Size(640, 480), CV_8UC3, (void*)color_frame.get_data(), 
        cv::Mat::AUTO_STEP);

    /* **************************************************
     * 图像处理
     * **************************************************/
    // Processing
    // Canny只接受单通道8位图像, 边缘检测前先将图像转换为灰度图
    cv::cvtColor(src, img, cv::COLOR_BGR2GRAY);         // 灰度图
    cv::GaussianBlur(img, img, cv::Size(5,5), 0, 0);    // 高斯滤波
    // Canny参数影响轮廓轮廓识别
    cv::Canny(img, img, 40, 80);

    // 霍夫变换提取直线
    HoughLinesP(img, tline,1,CV_PI/90,14,6,16);

    // 获取直线信息
    h_min = 99;
    flag = 0;
    for (size_t i = 0; i < tline.size(); i++) {
      // 计算斜率和高度
      delta_x = tline[i][2] - tline[i][0];
      delta_y = tline[i][3] - tline[i][1];
      if(delta_x == 0) {
        continue;
      }
      else{
        k = (double)(delta_y / delta_x);
        h = (double)((tline[i][1]+tline[i][3])/2*px2mm);
      }

      // 过滤得到水平直线
      if (k > 0.1) {continue;}

      if (h - h_min < 0) {
        h_min = h;
        id = i;
        flag = 1;
      }

    } // for

    // 返回检测到的工具上边缘
    if (flag) {
      cv::line(src, cv::Point(tline[id][0],tline[id][1]),
        cv::Point(tline[id][2],tline[id][3]),
        cv::Scalar(0, 0, 255),2,8);
    }

    cv::imshow("LineImage", src);

    // 等待n ms;如果有键盘输入则退出循环; 否则n ms后跳过并执行下一个循环
    // 在退出的时候,所有数据的内存空间将会被自动释放
    if(cv::waitKey(10) >= 0) break;
    if(shm_servo_inter.status_control == EXIT_C) break;
	}  // while

  std::cout << "Set Origin at [mm]: " << h_min << std::endl;
  cv::destroyWindow("LineImage");
  return h_min;
}  // setOrig



double getLine(rs2::frameset frames, rs2::pipeline pipe) {
  cv::Mat img;
  // 储存直线信息
  std::vector<cv::Vec4i> tline;
  double delta_x, delta_y, k, h;
  double h_min;
  int h_flag;    // 是否找到工具上边缘的标志

  frames = pipe.wait_for_frames();    // 等待下一帧

  // Get frames
  rs2::frame color_frame = frames.get_color_frame();

  // 创建Opencv中的Mat类,并传入数据
  cv::Mat src(cv::Size(640, 480), CV_8UC3, (void*)color_frame.get_data(),
      cv::Mat::AUTO_STEP);

  /* **************************************************
   * 图像处理
   * **************************************************/
  // Processing
  // Canny只接受单通道8位图像, 边缘检测前先将图像转换为灰度图
  cv::cvtColor(src, img, cv::COLOR_BGR2GRAY);         // 灰度图
  cv::GaussianBlur(img, img, cv::Size(5,5), 0, 0);    // 高斯滤波
  // Canny参数影响轮廓轮廓识别
  cv::Canny(img, img, 40, 80);

  // 霍夫变换提取直线
  HoughLinesP(img, tline,1,CV_PI/90,14,6,16);

  // 获取直线信息
  h_min = 99;    h_flag = 0;
  // 未读取到直线信息
  if(tline.size() == 0) {
    return -1;
  }
  // 读取到直线信息
  for (size_t i = 0; i < tline.size(); i++) {
    // 计算斜率和高度
    delta_x = tline[i][2] - tline[i][0];
    delta_y = tline[i][3] - tline[i][1];
    if(delta_x == 0) {continue;}
    else{
      k = (double)(delta_y / delta_x);
      h = (double)((tline[i][1]+tline[i][3])/2*px2mm);
    }

    // 过滤得到水平直线
    if (k > 0.1) {continue;}

    // 更新工具上边缘位置
    if (h - h_min < 0) {
      h_min = h;
      h_flag = 1;
    }
  } // for

  if (h_flag) {
    return h_min;
  }
  else {return -1;}
} // getLine

