/***********************************
 * 文件名称：rmctrl.h
 * 源 文 件：rmctrl.cpp
 * 功    能：定义夹爪类RmDriver;
 *           重写夹爪控制接口;
 ***********************************/

#ifndef RMCTRL_H
#define RMCTRL_H

#include "motion_axis.h"

class RmDriver {
  private:
    float curpos;             // 夹爪当前位置
    rm_axis_handle handle;    // 夹爪链接成功后的句柄

  public:
    /***********************************
     * 函数名称：RmDriver构造函数
     * 功    能：初始化RmDriver类
     * 参    数：串口号, 波特率, 夹爪编号(0)
     * 返 回 值：void
     ***********************************/
    RmDriver(const char* port, int baudrate, uint8_t axis_id);

    /***********************************
     * 函数名称：setMotion
     * 功    能：设置夹爪运动参数
     * 参    数：速度, 加速度, 减速度
     * 返 回 值：void
     ***********************************/
    void setMotion(float velocity, float acceleration, float deacceleration);

    /***********************************
     * 函数名称：setPos
     * 功    能：设置夹爪位置
     * 参    数：目标位置
     * 返 回 值：void
     ***********************************/
    void setPos(float position);

    /***********************************
     * 函数名称：getPos
     * 功    能：读取夹爪位置
     * 参    数：NULL
     * 返 回 值：float 夹爪当前位置
     ***********************************/
    float getPos();

    /***********************************
     * 函数名称：setPush
     * 功    能：以特定大小的力推动夹爪运动指定距离，如果阻力过大会停止
     * 参    数：电机出力百分比, 推动距离(闭合为正), 运动速度
     * 返 回 值：void
     ***********************************/
    void setPush(float force, float distance, float velocity);

    /***********************************
     * 函数名称：goHome
     * 功    能：将夹爪返回原点
     * 参    数：NULL
     * 返 回 值：void
     ***********************************/
    void goHome();

    /***********************************
     * 函数名称：close
     * 功    能：与夹爪断开链接
     * 参    数：NULL
     * 返 回 值：void
     ***********************************/
    void close();
};

#endif

