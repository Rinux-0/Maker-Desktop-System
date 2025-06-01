#include "demo_def.h"

#include "ddef.h"
#include "ttool.h"

#include "comm.h"
#include MY_HEADER_NAME(DEV_OR_TEST, h)

#include <app_init.h>
#include <cmsis_os2.h>



static void demo_init(void) {
	tool_init();

	comm_init();

	INIT(DEV_OR_TEST)();

	LOG("");
}


static void demo_loop(void) {
	tool_timer_start(0, 1024 * 2);		// demo_run 状态记录

	while (1) {
		tool_watchdog_kick();

		ONELOOP(DEV_OR_TEST)();

		comm_oneloop();
	}
}


static void demo_exit(void) {
	EXIT(DEV_OR_TEST)();

	comm_exit();

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
		.name = "MakerDesktop-System",
		.attr_bits = 0U,
		.cb_mem = NULL,
		.cb_size = 0U,
		.stack_mem = NULL,
		.stack_size = 0x5000,
		.priority = (osPriority_t)28,
		// .tz_module	= 0U,
		// .reserved	= 0U
	};

	if (NULL == osThreadNew((osThreadFunc_t)demo, NULL, &attr)) {

	}
}


app_run(demo_entry);
