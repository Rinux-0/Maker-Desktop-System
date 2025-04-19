#include "receiver.h"
#include "receiver_def.h"

#include "core/rcv_core.h"
#include "def.h"
#include "tool.h"



void receiver_init(void) {
	rcv_init_pin();
}


/// @todo 待完善 其他功能
void receiver_oneloop(void) {
	static bool new_change = false;
	static u8 time_led_on = 0;		// 记录LED持续轮数 -防止LED亮的时间太短，不便观测

	// tool_sleep_m(1);	// (多线程)

	// 0. 预处理
	if (new_change) {
		// 有变化 --> LED 开始亮
		tool_led_on();
		time_led_on = 1;
		new_change = false;
	} else if (time_led_on>0 && time_led_on++ > 2) {
		// 无变化 --> 1. 若 LED 持续亮ing --> 2. 若 LED 本次持续时间足够 --> LED 灭
		tool_led_off();
		time_led_on = 0;
	}


	// 1. 键态[现] ----更新到---> 键态[过]
	rcv_update_past();


	// 2. 电平 ----读取到---> 键态[现]
	rcv_read_now();


	// 3. 键态 -变化检测
	if (rcv_is_valid_diff()) {
		// 4. 键态[现] ----构造为---> 数据包 ----UART---> 发送
		rcv_set_hid_wp();
		rcv_send_hid_wp();
		new_change = true;
	}
}


void receiver_exit(void) {}
