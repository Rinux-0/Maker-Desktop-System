#include "def.h"
#include "device/device.h"
#include "ssle.h"
#include "tool.h"
#include "uuart.h"

#include <app_init.h>
#include <cmsis_os2.h>



static void demo_init(void) {
	tool_init();

	uart_init();
	sle_init();

	uart_set_r_cb(sle_uart_r_int_handler);
	sle_set_r_cb(uart_sle_r_int_handler);

	device_init();
}


static void demo_loop(void) {
	while (1) {
		// tool_sleep_m(1);	// (多线程)
		tool_watchdog_kick();

		device_oneloop();
		sle_oneloop();
	}
}


static void demo_exit(void) {
	device_exit();

	sle_exit();
	uart_exit();

	tool_exit();
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
		.name		= "MakerDesktop-System",
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
// app_run(sle_entry);	// (多线程)
