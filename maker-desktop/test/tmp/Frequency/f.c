#include "f.h"

#include "ddef.h"
#include "ttool.h"



void f_init(void) {
	// LOG("f_init");

	uapi_pin_set_mode(1, PIN_MODE_0);
	uapi_gpio_set_dir(1, GPIO_DIRECTION_OUTPUT);
	uapi_gpio_set_val(1, GPIO_LEVEL_LOW);
}


void f_oneloop(void) {
	while (1) {
		tool_delay_u(1);
		tool_pin_gpio_toggle(1);
	}
}


void f_exit(void) {
	// LOG("f_exit");
}
