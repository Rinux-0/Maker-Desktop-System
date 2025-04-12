#pragma once
#ifndef _UTIL_TOOL_H_
#define _UTIL_TOOL_H_



#include "util_def.h"

#include <pinctrl.h>
#include <stdio.h>
#include <string.h>

void mdelay(u32 time_ms);
void udelay(u32 time_us);
void watchdog_kick(void);

void gpio_refresh(pin_t pin, u32 time_us);
void led_on(void);
void led_off(void);
void led_toggle(void);

void util_tool_init(void);



#endif // _UTIL_TOOL_H_
