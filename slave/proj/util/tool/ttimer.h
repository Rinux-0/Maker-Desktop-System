#pragma once
#ifndef _TTIMER_H_
#define _TTIMER_H_



#include "ddef.h"
#include <timer.h>

#define TIMERS_NUM 5

typedef struct {
	u32 start;
	u32 end;
	u32 delay;
} timer_info_t;

extern volatile u64 g_time_wait_2s;
extern volatile bool g_time_wait_0s1;
extern volatile bool g_time_wait_0s25;
extern volatile bool g_time_wait_0s5;

void tool_timer_init(void);
void tool_timer_start_m(u8 timer_id, u16 time_delay_ms, timer_callback_t timeout_cb);
void tool_timer_start_u(u8 timer_id, u16 time_delay_us, timer_callback_t timeout_cb);
void tool_timer_stop(u8 timer_id);
// void tool_timer_end(u8 timer_id);
void tool_timer_exit(void);



#endif // _TTIMER_H_
