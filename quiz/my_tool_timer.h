#ifndef _MY_TOOL_TIMER_H_
#define _MY_TOOL_TIMER_H_

#include  "my_def.h"
#include  "my_tool.h"
#include  "main.h"
#include <timer.h>
#include <tcxo.h>
#include <chip_core_irq.h>
#include "my_tool_gpio.h"

#define TIMERS_NUM 4
#define TIMER_INDEX 1
#define TIMER_PRIO  1

typedef struct {
	u32 start;
	u32 end;
	u32 delay;
} timer_info_t;

void tool_timer_init(void);
void tool_timer_start_m(u8 timer_id, u16 time_delay_ms, timer_callback_t timeout_cb);
void tool_timer_start_u(u8 timer_id, u16 time_delay_us, timer_callback_t timeout_cb);
void tool_timer_stop(u8 timer_id);
void tool_timer_end(u8 timer_id);
void mytimer_callback(uintptr_t data);

#endif
