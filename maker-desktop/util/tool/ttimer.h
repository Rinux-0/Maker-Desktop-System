#pragma once
#ifndef _TTIMER_H_
#define _TTIMER_H_



#include "ddef.h"

typedef struct {
	u32 start;
	u32 end;
	u32 delay;
} timer_info_t;

extern u64 g_time_wait;

void tool_timer_init(void);
void tool_timer_start(u8 timer_id, u16 time_delay_ms);
// void tool_timer_stop(u8 timer_id);
void tool_timer_exit(void);



#endif // _TTIMER_H_
