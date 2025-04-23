#include "demo_def.h"

#include "ddef.h"
#include "ttool.h"

#include "comm.h"
#if defined(CONFIG_DEVICE)
#	include "device.h"
#elif defined(CONFIG_HEALTH)
#	include "health.h"
#endif

#include <app_init.h>
#include <cmsis_os2.h>



static void demo_init(void) {
	tool_init();

	comm_init();

	/* 单选 */
	/*1*/device_init();
	/*2*/health_init();
}


static void demo_loop(void) {
	while (1) {
		tool_watchdog_kick();

		/* 单选 */
		/*1*/device_oneloop();
		/*2*/health_oneloop();

		comm_oneloop();
	}
}


static void demo_exit(void) {
	/* 单选 */
	/*1*/device_exit();
	/*2*/health_exit();

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
