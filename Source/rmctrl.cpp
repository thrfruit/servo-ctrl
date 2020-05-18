/***********************************
 * 文件名称：rmctrl.cpp
 * 头 文 件：rmctrl.h
 * 功    能：定义夹爪类RmDriver及其成员函数
 ***********************************
 * TODO:
 * *********************************/

#include "../include/common.h"
#include "../include/rmctrl.h"

RmDriver::RmDriver(const char* port, int baudrate, uint8_t axis_id){
  rm_init();
  RmDriver::handle = rm_open_axis_modbus_rtu("/dev/ttyS110", 115200, 0);
  if(RmDriver::handle+1 == 0) {
    printf("handle = %d\n", handle);
    printf("Can't connect to RmDriver [%s]\n", port);
    exit(1);
  }
  else {
    rm_reset_error(RmDriver::handle);
    printf("Connect to RmDriver [%s]\n", port);
  }
}

void RmDriver::setMotion(float velocity, float acceleration, float deacceleration) {
  rm_config_motion(RmDriver::handle, velocity, acceleration, deacceleration);
}

void RmDriver::setPos(float position) {
  rm_set_position(RmDriver::handle, position);
}

float RmDriver::getPos() {
  RmDriver::curpos = rm_read_current_position(RmDriver::handle);
  return RmDriver::curpos;
}

void RmDriver::setPush(float force, float distance, float velocity) {
  rm_push(RmDriver::handle, force, distance, velocity);
}

void RmDriver::goHome() {
  rm_go_home(RmDriver::handle);
  while(rm_is_moving(RmDriver::handle));
}

void RmDriver::close() {
  rm_set_position(RmDriver::handle, 2);
  while(rm_is_moving(RmDriver::handle));
  rm_close_axis(RmDriver::handle);
  printf("Disconnect to RmDriver [%d]\n", RmDriver::handle);
}

