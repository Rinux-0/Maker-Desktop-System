#include "keyboard.h"
#include "keyboard_def.h"

#include "core/kbd_core.h"
#include "def.h"
#include "tool.h"




void keyboard_init(void) {
	kbd_pin_init();
}


/// @todo 待完善 其他功能
void keyboard_oneloop(void) {
	static bool new_change = false;
	static u8 time_led_on = 0;		// 记录LED持续轮数 -防止LED亮的时间太短，不便观测

	// m_sleep(1);	// 为了多线程切换

	// 0. 预处理
	if (new_change) {
		// 有变化 --> LED 开始亮
		led_on();
		time_led_on = 1;
		new_change = false;
	} else if (time_led_on>0 && time_led_on++ > 2) {
		// 无变化 --> 1. 若 LED 持续亮ing --> 2. 若 LED 本次持续时间足够 --> LED 灭
		led_off();
		time_led_on = 0;
	}


	// 1. 键态[现] ----更新到---> 键态[过]
	kbd_past_update();


	// 2. 电平 ----读取到---> 键态[现]
	kbd_now_read();


	// 3. 键态 -变化检测
	if (kbd_is_valid_diff()) {
		// 4. 键态[现] ----构造为---> 数据包 ----UART---> 发送
		kbd_hid_wp_construct();
		kbd_hid_wp_send();
		new_change = true;
	}
}


void keyboard_exit(void) {}
