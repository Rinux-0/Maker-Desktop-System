#include "keytest.h"
#include "keytest_def.h"

#include "color.h"
#include "ddef.h"
#include "ttool.h"

#include "core/ktt_core.h"
#include "comm.h"



void keytest_init(void) {
	ktt_init_pin();
	ktt_init_int_cb();

	LOG("");
}


void keytest_oneloop(void) {
	static bool new_change = false;
	static u8 time_led_on = 0;		// LED持续亮轮数

	// 灯光秀
	color_show(0, KTT_NUM_KEY, .125f);

	// 0. 预处理
	if (new_change) {
		// 有变化 --> LED 亮
		tool_pin_gpio_set_val(LED_PIN_RUN, 0);
		time_led_on = 1;
		new_change = false;
	} else if (time_led_on > 0 && time_led_on++ > 32) {
		// 无变化 且 LED 持续时间足够 --> LED 灭
		tool_pin_gpio_set_val(LED_PIN_RUN, 1);
		time_led_on = 0;
	}

	// 1. 键态 -更新
	ktt_update_past();
	ktt_read_now();

	// 2. 键态 -变化判定
	if (!ktt_is_valid_diff())
		return;

	// 3. 按键处理
	if (ktt_is_fn_pressed()) {
		ktt_fn_processer();
	} else {
		ktt_set_ktt_hid_wp();
		ktt_send_hid_wp();
	}

	new_change = true;
}


void keytest_exit(void) {}
