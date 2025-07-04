#include "health.h"
#include "health_def.h"

#include "ddef.h"
#include "ttool.h"

#ifdef CONFIG_DEVICE_HEALTH_DISTANCE
#	include "distance/distance.h"
#endif
#ifdef CONFIG_DEVICE_HEALTH_RATE
#	include "rate/rate.h"
#endif
#ifdef CONFIG_DEVICE_HEALTH_TEMPERATURE
#	include "temperature/temperature.h"
#endif



void health_init(void) {
	// /*1*/distance_init();
	// /*2*/rate_init();
	// /*3*/temperature_init();

	tool_timer_start(1, 1000 * 0.1);

	distance_entry();
	rate_entry();
	temperature_entry();
}


void health_oneloop(void) {
	tool_delay_m(1);

	// /*1*/distance_oneloop();
	// /*2*/rate_oneloop();
	// /*3*/temperature_oneloop();
}


void health_exit(void) {
	// /*1*/distance_exit();
	// /*2*/rate_exit();
	// /*3*/temperature_exit();
}
