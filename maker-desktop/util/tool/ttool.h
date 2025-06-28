#pragma once
#ifndef _UTIL_TTOOL_H_
#define _UTIL_TTOOL_H_



#include "ddef.h"
#include "ttimer.h"
#include <gpio.h>
#include <pinctrl.h>
#include <soc_osal.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DATA(args...)	osal_printk(args);
#define POSITION()		osal_printk("[F]%s\n[f]%s\n[L]%d\n", __F_NAME__, __func__, __LINE__);

#define INIT(obj)		CONCAT2(obj, _init)
#define ONELOOP(obj)	CONCAT2(obj, _oneloop)
#define EXIT(obj)		CONCAT2(obj, _exit)

#if defined(OUTPUT_DEBUG)
#	define LOG(args...) { \
		osal_printk("[###@LOG@###]\n"); \
		POSITION(); \
		DATA(args); \
	}
#	define ERROR(args...) { \
		osal_printk("[###@ERROR@###]\n"); \
		POSITION(); \
		DATA(args); \
	}
extern	u64 time_debug;
#	define DEBUG(args...) { \
		osal_printk("[###@DEBUG@###]\n"); \
		POSITION(); \
		DATA("[T]%llu\n", time_debug++); \
		DATA(args); \
	}
#elif defined(OUTPUT_RELEASE)
#	define LOG(args...)
#	define ERROR(args...)
#	define DEBUG(args...)
#endif

void tool_tcxo_init(void);
u64 tool_tcxo_get_m(void);
bool tool_tcxo_is_timeout(u64 timeout_ms, u64 start_time_ms);

void tool_delay_u(u16 time_us);
void tool_delay_m(u16 time_ms);
// void tool_sleep_m(u16 time_ms);
void tool_watchdog_kick(void);

void tool_pin_gpio_init(pin_t pin, gpio_direction_t dir, gpio_level_t level);
void tool_pin_gpio_set_val(pin_t pin, gpio_level_t level);
void tool_pin_gpio_toggle(pin_t pin);
void tool_pin_gpio_refresh_u(pin_t pin, u16 time_us);
void tool_pin_gpio_refresh_m(pin_t pin, u16 time_ms);

void tool_led_run_toggle(void);

void tool_init(void);
void tool_oneloop(void);
void tool_exit(void);



c8* strnstr(const c8* s1, const c8* s2, size_t max_size);



#endif // _UTIL_TTOOL_H_
