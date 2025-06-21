#include "health.h"
#include "health_def.h"

#include "ddef.h"
#include "ttool.h"

#if defined(CONFIG_DEVICE_HEALTH_DISTANCE)
#	include "distance/distance.h"
#endif
#if defined(CONFIG_DEVICE_HEALTH_RATE)
#	include "rate/rate.h"
#endif
#if defined(CONFIG_DEVICE_HEALTH_TEMPERATURE)
#	include "temperature/temperature.h"
#endif



void health_init(void) {
	/*1*/distance_init();
	/*2*/rate_init();
	/*3*/temperature_init();
}


void health_oneloop(void) {
	/*1*/distance_oneloop();
	/*2*/rate_oneloop();
	/*3*/temperature_oneloop();
}


void health_exit(void) {
	/*1*/distance_exit();
	/*2*/rate_exit();
	/*3*/temperature_exit();
}
