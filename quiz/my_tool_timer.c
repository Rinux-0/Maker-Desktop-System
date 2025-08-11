#include "my_tool_timer.h"




timer_handle_t timer_hdl[TIMERS_NUM] = { 0 };

timer_info_t g_timers_info[TIMERS_NUM] = {
	{0, 0, 0},
	{0, 0, 0},
	{0, 0, 0},
	{0, 0, 0},
};



void mytimer_callback(uintptr_t data) {
	u32 timer_index = (u32)data;
	g_timers_info[timer_index].end = uapi_tcxo_get_ms();

	switch (timer_index) {
	default:
	break;case 0:tool_timer_start_m(0, 10, mytimer_callback);
				keyscan();
	break;case 1:tool_timer_start_m(1, 1000, mytimer_callback);
				uapi_gpio_toggle(10);
	break;case 2:tool_timer_start_m(2, 10, mytimer_callback);

	break;case 3:tool_timer_start_m(3, 10, mytimer_callback);

				break;
	}
}


void tool_timer_init(void) {
	uapi_timer_init();
	uapi_timer_adapter(TIMER_INDEX, TIMER_1_IRQN, TIMER_PRIO);

	for (u8 i = 0; i < TIMERS_NUM; i++)
		uapi_timer_create(TIMER_INDEX, &timer_hdl[i]);
}

//timer_id:0-3  cb:mytimer_callback
void tool_timer_start_m(u8 timer_id, u16 time_delay_ms, timer_callback_t timeout_cb) {
	g_timers_info[timer_id].delay = time_delay_ms;
	g_timers_info[timer_id].start = uapi_tcxo_get_ms();
	uapi_timer_start(timer_hdl[timer_id], time_delay_ms * 1000, timeout_cb, timer_id);
}

//timer_id:0-3  cb:mytimer_callback
void tool_timer_start_u(u8 timer_id, u16 time_delay_us, timer_callback_t timeout_cb) {
	g_timers_info[timer_id].delay = time_delay_us;
	g_timers_info[timer_id].start = uapi_tcxo_get_us();
	uapi_timer_start(timer_hdl[timer_id], time_delay_us, timeout_cb, timer_id);
}

//timer_id:0-3
void tool_timer_stop(u8 timer_id) {
	uapi_timer_stop(timer_hdl[timer_id]);
}

//timer_id:0-3
void tool_timer_end(u8 timer_id) {
	if (timer_hdl[timer_id] != NULL)
		uapi_timer_delete(timer_hdl[timer_id]);
}
