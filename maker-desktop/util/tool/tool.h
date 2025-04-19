#pragma once
#ifndef _UTIL_TOOL_H_
#define _UTIL_TOOL_H_



#include "def.h"

#include <pinctrl.h>
#include <soc_osal.h>
#include <stdio.h>
#include <string.h>

// #define __FILE_R__ (strstr(__FILE__, "maker-desktop") + sizeof("maker-desktop"))	// 相对路径
#define __F_NAME__ (strrchr(__FILE__, '/') + 1)										// 文件名

#if defined(CONFIG_DEBUG)
#	define LOG(args...) { \
		osal_printk("[ LOG ]\n[F]%s\n[f]%s\n[L]%d\n", __F_NAME__, __func__, __LINE__), \
		osal_printk(args); \
	}
#	define ERROR(args...) { \
		osal_printk("[ ERROR ]\n[F]%s\n[f]%s\n[L]%d\n", __F_NAME__, __func__, __LINE__), \
		osal_printk(args); \
	}
#	if 1
extern	u64 time_debug;
#		define DEBUG(args...) { \
			osal_printk("[ DEBUG ]\n[F]%s\n[f]%s\n[L]%d\n[T]%lld\n", __F_NAME__, __func__, __LINE__, time_debug++), \
			osal_printk(args); \
		}
#	else
#		define DEBUG(args...)
#	endif
#elif defined(CONFIG_RELEASE)
#	define LOG(args...)
#	define ERROR(args...)
#	define DEBUG(args...)
#endif

u64 tool_tcxo_get_m(void);
bool tool_tcxo_is_timeout(u64 timeout_ms, u64 start_time_ms);
void tool_sleep_m(u16 time_ms);
void tool_delay_u(u16 time_us);
void tool_watchdog_kick(void);

void tool_gpio_refresh(pin_t pin, u32 time_us);

#if LED_PIN != PIN_NONE
void tool_led_on(void);
void tool_led_off(void);
void tool_led_toggle(void);
#else
#	define tool_led_on()
#	define tool_led_off()
#	define tool_led_toggle()
#endif

void tool_init(void);
void tool_exit(void);



#endif // _UTIL_TOOL_H_
