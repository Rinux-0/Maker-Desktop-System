#include "my_tool_timer.h"

timer_handle_t timer_hdl[TIMERS_NUM] = { 0 };

timer_info_t g_timers_info[TIMERS_NUM] = {
	{0, 0, 0},
	{0, 0, 0},
	{0, 0, 0},
	{0, 0, 0},
};

uint8_t RGB_color_now = 0;
uint8_t RGB_Shift = 0;
uint8_t RGB_duty = 0;
bool RGB_dir = 0;
void mytimer_callback(uintptr_t data) {
	u32 timer_index = (u32)data;
	g_timers_info[timer_index].end = uapi_tcxo_get_ms();

	switch (timer_index) {
	default:
	break;case 0:tool_timer_start_m(0, 10, mytimer_callback);
				keyscan();

	break;case 1:tool_timer_start_m(1, 1, mytimer_callback);
				if (RGB_state != 5)break;
				RGB_test1(RGB_Shift++);
				if (RGB_Shift >= 248)RGB_Shift = 0;

	break;case 2:tool_timer_start_m(2, 2, mytimer_callback);
				if(RGB_state == 6){
					RGB_test2(RGB_duty);
					if(RGB_dir == 0){
						if(RGB_duty<250){
							RGB_duty++;
						}
						else{
							RGB_dir = 1;
						}
					}else{
						if(RGB_duty>0){
							RGB_duty--;
						}else{
							RGB_dir = 0;
						}
					}
				}
				if(RGB_state == 7)RGB_test3(adc_data);


	break;case 3:tool_timer_start_m(3, 1000, mytimer_callback);
				if (RGB_state != 4)break;

				switch (RGB_color_now) {
				case 0:RGB_singlershow(RGB_RED);RGB_color_now++;break;
				case 1:RGB_singlershow(RGB_GREEN);RGB_color_now++;break;
				case 2:RGB_singlershow(RGB_BLUE);RGB_color_now = 0;break;
				}
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
