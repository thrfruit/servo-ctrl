#include <motion_axis.h>

void main()
{
	// 初始化sdk库
	rm_init();

	// 打开modbus rtu轴
	rm_axis_handle handle = rm_open_axis_modbus_rtu("/dev/ttyS110", 115200, 0);

	// 重置错误
	rm_reset_error(handle);

	// 上电后需要回原点一次
	rm_go_home(handle);

	// 等待回原点
	while (rm_is_moving(handle));

	// 往返运动2次
	for (int i = 0; i < 2; i++)
	{
		// 绝对运动
		rm_move_absolute(handle, 0, 10, 3000, 3000, 0.1);

		// 等待绝对运动到位
		while (!rm_is_position_reached(handle));

		// 绝对运动
		rm_move_absolute(handle, 10, 10, 3000, 3000, 0.1);

		// 等待绝对运动到位
		while (!rm_is_position_reached(handle));
	}

	// 推压运动
	rm_push(handle, 50, 10, 10);

	// 等待推压到位
	while (!rm_is_position_reached(handle));

	// 判断是否夹持/推压到物体
	bool is_holding = rm_is_moving(handle);

	// 绝对运动
	rm_move_absolute(handle, 0, 10, 3000, 3000, 0.1);
	rm_push(handle, 50, 10, 10);

	// 断开轴
	rm_close_axis(handle);
}

