#ifndef _UTIL_TOOL_H_
#define _UTIL_TOOL_H_



#include <gpio.h>		// gpio相关
#include <pinctrl.h>	// pin相关
#include "util_def.h"

void util_tool_init(void);
void mdelay(u32 time_ms);
void udelay(u32 time_us);

void pin_voltage_refresh(pin_t pin, u32 time_us);

void led_on(void);
void led_off(void);
void led_toggle(void);

void util_tool_init(void);



#endif /* _UTIL_TOOL_H_ */
