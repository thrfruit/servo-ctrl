# ======================================== 
# 工程名
# ======================================== 
project(SERVO)

# ======================================== 
# Cmake最低版本需求
# ======================================== 
cmake_minimum_required(VERSION 3.0)

# ======================================== 
# 确定编译语言
# ======================================== 
set(CMAKE_CXX_COMPILER g++)    # 解决对'@GLIBCXX_3.4.20'未定义的引用的问题
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")##解决ISO C++ 2011  的问题

# ======================================== 
# 确定需要的头文件所在文件夹
# ======================================== 
include_directories(${CMAKE_CURRENT_LIST_DIR}/include)

# ======================================== 
# 多个文件相关，添加源代码
# ======================================== 
aux_source_directory(./Source DIR_SRCS)

# ======================================== 
# 动态链接库路径
# ======================================== 
link_directories(${CMAKE_CURRENT_LIST_DIR}/lib)

# ======================================== 
# 生成可执行文件
# ======================================== 
add_executable(servo ${DIR_SRCS})

# ======================================== 
# 添加依赖库
# ======================================== 
## 解决pthread_create’未定义的引用的问题 
find_package(Threads)
target_link_libraries(servo ${CMAKE_THREAD_LIBS_INIT})

target_link_libraries(servo -lMotorMaster.Sdk)    # 夹爪的链接库
target_link_libraries(servo -l_usbdaq_v20 -lusb-1.0)    # AD采集板卡的链接库

## Realsense
find_package(OpenCV REQUIRED)
target_link_libraries(servo ${OpenCV_LIBS})

## OpenCV
# 前两条命令等同于第三条命令
set(DEPENDENCIES realsense2)
target_link_libraries(servo ${DEPENDENCIES})
# target_link_libraries(demo -lrealsense2)    # Link lib of Realsense


