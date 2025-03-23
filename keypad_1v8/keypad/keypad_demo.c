#include "keypad_demo.h"

#include <stdbool.h>	// bool相关
#include <stddef.h>		// NULL
#include <stdio.h>		// sprintf()

#include "cfg/cfg.h"		// cfg相关
#include "comm/hid/hid.h"	// hid相关
#include "comm/uart/uart.h"	// uart相关
#include "keypad/keypad.h"	// keypad相关
#include "util/util_tool.h"	// led相关



void keypad_demo_init(void) {
	/* --Watchdog-- */
	watchdog_init();

	/* --util_tool-- */
	util_tool_init();

	/* --KEYPAD-- */
	keypad_init();

	/* --UART-- */
	uart_init();
	debug_uart_init();
}


/// @todo 待完善 其他功能
void keypad_demo_oneloop(void) {
	static bool new_change = false;
	static u8 time_led_on = 0;		// 记录LED持续轮数 -防止LED亮的时间太短，不便观测


	// 0. 预处理
	if (new_change) {
		// 有变化 --> LED 开始亮
		led_on();
		time_led_on = 1;
		new_change = false;
	} else if (time_led_on>0 && time_led_on++ > 32) {
		// 无变化 --> 1. 若 LED 持续亮ing --> 2. 若 LED 本次持续时间足够 --> LED 灭
		led_off();
		time_led_on = 0;
	}


	// 1. 键态[现] ----更新到---> 键态[过]
	keypad_past_update();


	// 2. 电平 ----读取到---> 键态[现]
	keypad_now_read();


	// 3. 键态 -变化检测
	if (keypad_is_valid_diff()) {
		// 4. 键态[现] ----构造为---> 数据包 ----发送到---> “上位机”
		hid_write_pack_construct(CMD_SEND_KB_GENERAL_DATA);
		uart_send_hid_pack();
		new_change = true;
	}


	// ...
}
