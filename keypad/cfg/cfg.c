#include "cfg.h"

#include <watchdog.h>	// watchdog相关



void watchdog_init(void) {
	// 看门狗（ms）
	uapi_watchdog_deinit();
	uapi_watchdog_init(3000);
	uapi_watchdog_disable();
	uapi_watchdog_enable(WDT_MODE_RESET);
}


void watchdog_kick(void) {
	uapi_watchdog_kick();
}
