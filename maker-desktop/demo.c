#include "device.h"
#include "util_tool.h"
#include "uuart.h"

#include <app_init.h>
#include <cmsis_os2.h>



static void demo_init(void) {
	device_init();
	uart_init();
	util_tool_init();
}


static void demo_loop(void) {
	while (1) {
		watchdog_kick();
		device_oneloop();
	}
}


static void demo_exit(void) {
	device_exit();
}


static void* demo(const char* arg) {
	unused(arg);

	demo_init();

	demo_loop();

	demo_exit();

	return NULL;
}


static void demo_entry(void) {
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

	if (NULL == osThreadNew((osThreadFunc_t)demo, NULL, &attr)) {

	}
}


app_run(demo_entry);
