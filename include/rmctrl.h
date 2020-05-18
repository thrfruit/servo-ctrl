
#ifndef RMCTRL_H
#define RMCTRL_H

#include"motion_axis.h"

class RmDriver {
  private:
    float curpos;

  public:
    rm_axis_handle handle;

    /* 构造函数 */
    RmDriver(const char* port, int baudrate, uint8_t axis_id);

    /* 设置夹爪运动参数 */
    void setMotion(float velocity, float acceleration, float deacceleration);

    /* 设置夹爪位置 */
    void setPos(float position);

    /* 读取夹爪位置 */
    float getPos();

    /* 推压运动 */
    void setPush(float force, float distance, float velocity);

    /* 回原点 */
    void goHome();

    /* 断开链接 */
    void close();
};

#endif

