# 工程名
project(SERVO)

# Cmake最低版本需求
cmake_minimum_required(VERSION 3.0)

# 确定编译语言
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")##解决ISO C++ 2011  的问题

# 确定需要的头文件所在文件夹
include_directories(${CMAKE_CURRENT_LIST_DIR}/include)

# 多个文件相关，添加源代码
aux_source_directory(./Source DIR_SRCS)

# 动态链接库路径
link_directories(${CMAKE_CURRENT_LIST_DIR}/lib)

add_executable(servo ${DIR_SRCS})

# 添加依赖库
## 解决pthread_create’未定义的引用的问题 
find_package(Threads)
target_link_libraries(servo ${CMAKE_THREAD_LIBS_INIT})

target_link_libraries(servo -lMotorMaster.Sdk)

