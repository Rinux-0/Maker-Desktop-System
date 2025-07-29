#include "bt.h"

#include "ddef.h"
#include "ttool.h"

#include "../core/snd_core.h"



#define SND_BT_GPIO_PIN 0



static bool snd_bt_is_connected;



static void snd_bt_timer_timeout_cb(uintptr_t data) {
	unused(data);

	if (1 == uapi_gpio_get_val(SND_BT_GPIO_PIN))
		snd_bt_is_connected = true;

	LOG("");
}


static void snd_bt_gpio_int_handler(pin_t pin, uintptr_t param) {
	unused(pin);
	unused(param);

	snd_bt_is_connected = false;

	tool_timer_stop(4);
	tool_timer_start_m(4, 1000 * 2, snd_bt_timer_timeout_cb);

	LOG("");
}


void snd_bt_init_pin(void) {
	// bt 连接状态
	uapi_pin_set_mode(SND_BT_GPIO_PIN, 0);
	uapi_gpio_set_dir(SND_BT_GPIO_PIN, GPIO_DIRECTION_INPUT);
	uapi_gpio_register_isr_func(SND_BT_GPIO_PIN, GPIO_INTERRUPT_RISING_EDGE, snd_bt_gpio_int_handler);

	uapi_gpio_enable_interrupt(SND_BT_GPIO_PIN);
}


bool snd_bt_is_conn(void) {
	return snd_bt_is_connected;
}
