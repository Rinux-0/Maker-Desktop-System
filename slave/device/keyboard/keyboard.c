#include "keyboard.h"
#include "keyboard_def.h"

#include "ddef.h"
#include "ttool.h"

#include "color.h"
#include "comm.h"

#include "core/kbd_core.h"
#include "oled/kbd_oled.h"
#include "oled/oled_font.h"



void keyboard_init(void) {
	kbd_init_pin();
	kbd_init_int_cb();

	OLED_Init_1309();

	// tool_timer_start_m();

	/// @todo tmp
	// uapi_pin_set_mode(LED_PIN_SLE, PIN_MODE_0);
	// uapi_gpio_set_dir(LED_PIN_SLE, GPIO_DIRECTION_OUTPUT);
	// uapi_gpio_set_val(LED_PIN_SLE, GPIO_LEVEL_HIGH);

	LOG("");
}


static void keyboard_oled_show(void) {
	static u8 mode = 0;
	static u64 now = 0;
	if (now == g_time_wait_2s)
		return;
	now = g_time_wait_2s;

	if (mode++ > 3)
		mode = 0;

	switch (mode) {
	default:
	break;case 0:OLED_ShowPix(nearlink);
	break;case 1:OLED_ShowPix(Hello_AI);
	break;case 2:OLED_ShowPix(his_and_AI);
	break;case 3:OLED_ShowPix(elab);
	}
}


void keyboard_oneloop(void) {
	static bool new_change = false;
	static u8 time_led_on = 0;		// LED持续亮轮数

	// 灯光秀
	color_show_0(KBD_NUM_KEY, .625f);

	// oled
	keyboard_oled_show();

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
	kbd_update_past();
	kbd_read_now();

	// 2. 键态 -变化判定
	if (!kbd_is_valid_diff())
		return;

	// 3. 按键处理
	if (kbd_is_fn_pressed()) {
		kbd_fn_processer();
	} else {
		kbd_set_kbd_hid_wp();
		// if (kbd_merge_hid_wp())
		kbd_send_hid_wp(NULL);
	}

	new_change = true;
}


void keyboard_exit(void) {}
