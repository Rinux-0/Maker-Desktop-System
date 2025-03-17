#include <app_init.h>	// app_run()
#include <cmsis_os2.h>	// os_thread相关
#include <stdbool.h>	// bool相关
#include <stddef.h>		// NULL
#include <stdio.h>		// sprintf()

#include "cfg/cfg.h"		// cfg相关
#include "comm/hid/hid.h"	// hid相关
#include "comm/uart/uart.h"	// uart相关
#include "keypad/keypad.h"	// keypad相关
#include "util/util_tool.h"	// led相关



static void task_init(void) {
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
/// @attention 暂时只处理主键盘功能，其它外设不一定在此处理（考虑多线程）
static void task_loop(void) {
	bool newChange = false;
	u8 time_led_on = 0;		// 记录LED持续轮数 -防止LED亮的时间太短，不便观测

	while (1) {
		watchdog_kick();


		// 0. 预处理
		if (newChange) {
			// 有变化 --> LED 开始亮
			led_on();
			time_led_on = 1;
			newChange = false;
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
			newChange = true;
		}


		// ...
	}
}


static void* keypad_task(const char* arg) {
	unused(arg);

	task_init();

	task_loop();

	return NULL;
}


static void keypad_entry(void) {
	osThreadAttr_t attr = {
		.name		= "Keypad-Task",
		.attr_bits	= 0U,
		.cb_mem		= NULL,
		.cb_size	= 0U,
		.stack_mem	= NULL,
		.stack_size	= 0x1000,
		.priority	= (osPriority_t)17,
		// .tz_module	= 0U,
		// .reserved	= 0U
	};

	if (osThreadNew((osThreadFunc_t)keypad_task, NULL, &attr) == NULL) {

	}
}


app_run(keypad_entry);
