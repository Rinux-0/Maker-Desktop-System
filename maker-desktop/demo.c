#include "demo_def.h"

#include "ddef.h"
#include "ttool.h"

#include "comm.h"
#include MY_HEADER_NAME(DEMO_NAME, h)
#ifdef CONFIG_COMM_SPI
#	include "color.h"
#endif

#include <app_init.h>
#include <cmsis_os2.h>



static void demo_init(void) {
	// comm_entry();

	tool_init();

	comm_init();

	// color_init();

	INIT(DEMO_NAME)();
}


static void demo_loop(void) {
	tool_timer_start_m(0, 1000 * 2, NULL);		// demo_run 状态记录

	while (1) {
		tool_watchdog_kick();

		ONELOOP(DEMO_NAME)();

		comm_oneloop();

		// tool_oneloop();

		// color_oneloop();
	}
}


static void demo_exit(void) {
	EXIT(DEMO_NAME)();

	comm_exit();

	tool_exit();

	// color_exit();
}


static void* demo(const c8* arg) {
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
		.stack_size = 0x1000,
		.priority = (osPriority_t)28,
		// .tz_module	= 0U,
		// .reserved	= 0U
	};

	if (NULL == osThreadNew((osThreadFunc_t)demo, NULL, &attr)) {
		ERROR("Failed to create MakerDesktop-System thread");
	}
}


app_run(demo_entry);
