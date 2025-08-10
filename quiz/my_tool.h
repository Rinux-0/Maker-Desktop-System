#pragma once
#ifndef _MY_TOOL_H_
#define _MY_TOOL_H_



#include  "my_def.h"



#include <gpio.h>
#include <pinctrl.h>
#include <soc_osal.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



#define DATA(args...)	osal_printk(args);
#define POSITION()		osal_printk("[F]%s\n[f]%s\n[L]%d\n", __F_NAME__, __func__, __LINE__);



#if 1
#	define LOG(args...) { \
		osal_printk("\n[###@LOG@###]\n"); \
		POSITION(); \
		DATA(args); \
	}
#	define ERROR(args...) { \
		osal_printk("\n[###@ERROR@###]\n"); \
		POSITION(); \
		DATA(args); \
	}
#else
#	define LOG(args...)
#	define ERROR(args...)
#	define DEBUG(args...)
#endif



void my_tool_delay_u_nop(u16 time_us);
void my_tool_delay_u(u16 time_us);
void my_tool_delay_m(u16 time_ms);
void my_tool_sleep_m(u16 time_ms);



c8* strnstr(const c8* s1, const c8* s2, size_t max_size);



void my_adc_init(void);
// void my_adc_(void);

void my_dog_init(u8 time_s);
void my_dog_kick(void);

void my_gpio_init(void);
// void my_gpio_(void);

void my_pwm_init(void);
// void my_pwm_(void);

void my_spi_init(void);
// void my_spi_(void);



#endif
