#include "ttool.h"

#include "ddef.h"

#include <timer.h>
#include <tcxo.h>
#include <chip_core_irq.h>



typedef struct timer_info {
	uint32_t start_time;
	uint32_t end_time;
	uint32_t delay_time;
} timer_info_t;



#define TIMER_INDEX 1
#define TIMER_PRIO  1

static timer_info_t g_timers_info[TIMERS_NUM] = {
	{0, 0, 0},		// demo_run 记录专用
	{0, 0, 0},
	{0, 0, 0},
	{0, 0, 0}
};
timer_handle_t timer_hdl[TIMERS_NUM] = { 0 };



static void timer_timeout_cb(uintptr_t data) {
	u32 timer_index = (u32)data;
	g_timers_info[timer_index].end_time = uapi_tcxo_get_ms();

	if (timer_index == 0) {
		tool_led_run_toggle();
		tool_timer_start(0, 1000 * 2);

		static u64 time_wait = 0;
		DATA("%llu\t", time_wait++);
	} else {
		g_timers_info[timer_index].delay_time = 0;
	}
}

void tool_timer_init() {
	uapi_timer_init();
	uapi_timer_adapter(TIMER_INDEX, TIMER_1_IRQN, TIMER_PRIO);

	for (u8 i = 0; i < TIMERS_NUM; i++)
		uapi_timer_create(TIMER_INDEX, &timer_hdl[i]);
}



void tool_timer_start(u8 timer_id, u16 time_delay_ms) {
	g_timers_info[timer_id].delay_time = time_delay_ms;
	g_timers_info[timer_id].start_time = uapi_tcxo_get_ms();

	uapi_timer_start(timer_hdl[timer_id], time_delay_ms * 1000, timer_timeout_cb, timer_id);
}


// void tool_timer_stop(u8 timer_id) {
// 	uapi_timer_stop(timer_hdl[timer_id]);
// }


void tool_timer_exit(void) {
	for (u8 i = 0; i < TIMERS_NUM; i++)
		if (timer_hdl[i] != NULL)
			uapi_timer_delete(timer_hdl[i]);
}
