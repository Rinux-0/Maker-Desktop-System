#include "ttimer.h"

#include "ddef.h"
#include "ttool.h"

#include <timer.h>
#include <tcxo.h>
#include <chip_core_irq.h>



#define TIMER_INDEX 1
#define TIMER_PRIO  1

volatile u64 g_time_wait_2s;
volatile bool g_time_wait_0s1;
volatile bool g_time_wait_0s25;
volatile bool g_time_wait_0s5;

/// @note 可能最多只能4个一起工作
timer_handle_t timer_hdl[TIMERS_NUM] = { 0 };
static timer_info_t g_timers_info[TIMERS_NUM] = {
	{0, 0, 0},		// 2s		// demo_run 记录专用
	{0, 0, 0},		// 0.1s
	{0, 0, 0},		// 0.25s
	{0, 0, 0},		// 0.5s
	{0, 0, 0},
};



static void timer_timeout_cb(uintptr_t data) {
	u32 timer_index = (u32)data;
	g_timers_info[timer_index].end = uapi_tcxo_get_ms();

	switch (timer_index) {
	default:	ERROR("\n\tt[%d]\n\n", timer_index);
	break;case 0:
		tool_led_run_toggle();
		tool_timer_start_m(0, 1000 * 2, NULL);
		DATA("%llu\t", g_time_wait_2s++);
	break;case 1:
		tool_timer_start_m(1, 1000 * 0.1, NULL);
		g_time_wait_0s1 = !g_time_wait_0s1;
	break;case 2:
		tool_timer_start_m(2, 1000 * 0.25, NULL);
		g_time_wait_0s25 = !g_time_wait_0s25;
	break;case 3:
		tool_timer_start_m(3, 1000 * 0.5, NULL);
		g_time_wait_0s5 = !g_time_wait_0s5;
	break;case 4:
		ERROR("timer[4] shuold enter its own cb");
	}
}


void tool_timer_init() {
	uapi_timer_init();
	uapi_timer_adapter(TIMER_INDEX, TIMER_1_IRQN, TIMER_PRIO);

	for (u8 i = 0; i < TIMERS_NUM; i++)
		uapi_timer_create(TIMER_INDEX, &timer_hdl[i]);
}


void tool_timer_start_m(u8 timer_id, u16 time_delay_ms, timer_callback_t timeout_cb) {
	g_timers_info[timer_id].delay = time_delay_ms;
	g_timers_info[timer_id].start = uapi_tcxo_get_ms();

	if (timeout_cb == NULL) {
		uapi_timer_start(timer_hdl[timer_id], time_delay_ms * 1000, timer_timeout_cb, timer_id);
	} else {
		uapi_timer_start(timer_hdl[timer_id], time_delay_ms * 1000, timeout_cb, timer_id);
	}
}


void tool_timer_start_u(u8 timer_id, u16 time_delay_us, timer_callback_t timeout_cb) {
	g_timers_info[timer_id].delay = time_delay_us;
	g_timers_info[timer_id].start = uapi_tcxo_get_us();

	uapi_timer_start(timer_hdl[timer_id], time_delay_us, timeout_cb, timer_id);
}


void tool_timer_stop(u8 timer_id) {
	uapi_timer_stop(timer_hdl[timer_id]);
}


// void tool_timer_end(u8 timer_id) {
// 	if (timer_hdl[timer_id] != NULL)
// 		uapi_timer_delete(timer_hdl[timer_id]);
// }



void tool_timer_exit(void) {
	for (u8 i = 0; i < TIMERS_NUM; i++)
		if (timer_hdl[i] != NULL)
			uapi_timer_delete(timer_hdl[i]);
}
