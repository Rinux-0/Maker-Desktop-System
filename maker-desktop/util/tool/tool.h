#pragma once
#ifndef _UTIL_TOOL_H_
#define _UTIL_TOOL_H_



#include "def.h"

#include <pinctrl.h>
#include <stdio.h>
#include <string.h>

void m_sleep(u16 time_ms);
void u_delay(u16 time_us);
void watchdog_kick(void);

void gpio_refresh(pin_t pin, u32 time_us);

#if LED_PIN != PIN_NONE
void led_on(void);
void led_off(void);
void led_toggle(void);
#else
#	define led_on()
#	define led_off()
#	define led_toggle()
#endif

void util_tool_init(void);



#endif // _UTIL_TOOL_H_
