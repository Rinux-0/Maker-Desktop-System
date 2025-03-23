#include <app_init.h>
#include <cmsis_os2.h>

#include "keypad/keypad_demo.h"
#include "sle_uart_1v8/sle_uart_1v8_demo.h"
#include "keypad/util/util_def.h"
#include "keypad/util/util_tool.h"
#include "keypad/cfg/cfg.h"



static void task_init(void) {
	keypad_demo_init();
	sle_uart_1v8_demo_init();
}


static void task_loop(void) {
	u8 is_nearlink_connected = 0;

	while (1) {
		watchdog_kick();

		if (is_nearlink_connected) {
			keypad_demo_oneloop();
			led_toggle();
			mdelay(500);
		} else {
			sle_uart_1v8_demo_oneloop();
			led_toggle();
			mdelay(5000);
		}
	}
}


static void task_exit(void) {
	sle_uart_1v8_demo_exit();
}


static void* task(const char* arg) {
	unused(arg);

	task_init();

	task_loop();

	task_exit();

	return NULL;
}


static void task_entry(void) {
	osThreadAttr_t attr = {
		.name		= "Keypad_1v8 - Task",
		.attr_bits	= 0U,
		.cb_mem		= NULL,
		.cb_size	= 0U,
		.stack_mem	= NULL,
		.stack_size	= 0x3000,
		.priority	= (osPriority_t)28,
		// .tz_module	= 0U,
		// .reserved	= 0U
	};

	if (osThreadNew((osThreadFunc_t)task, NULL, &attr) == NULL) {

	}
}


app_run(task_entry);
