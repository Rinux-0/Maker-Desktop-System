#include "sound.h"

#include "ddef.h"
#include "ttool.h"

#include "core/snd_core.h"
#include "uuart.h"



static bool is_wating;



static void sound_uart_r_int_handler(const void* buffer, u16 length, bool error) {
	unused(error);

	if (is_wating) {
		LOG("\n\tLast snd cmd is processing ...\n\n");
		return;
	}

	is_wating = true;

	c8* buff = (c8*)buffer;
	c8* substr = strnstr(buff, "snd", length);
	if (substr == NULL) {
		DATA("\n\tcmd error !\n\n");
		is_wating = false;
		return;
	}

	DATA("\n\tGet snd cmd[%u]: [ ", length);
	substr += sizeof("snd");
	length -= sizeof("snd");

	tool_pin_gpio_set_val(0, 1);
	tool_pin_gpio_set_val(1, 1);
	tool_pin_gpio_set_val(2, 1);

	if (false == snd_core_process_cmd(substr, length))
		DATA("ERROR... ]\n\n");

	tool_pin_gpio_set_val(0, 1);
	tool_pin_gpio_set_val(1, 1);
	tool_pin_gpio_set_val(2, 1);

	is_wating = false;
}


void sound_init(void) {
	uart_set_r_cb(UART_BUS_ID(1), sound_uart_r_int_handler);
	snd_core_init_pin();
}


void sound_oneloop(void) {
	// LOG("sound_oneloop\n");
}


void sound_exit(void) {
	// LOG("sound_exit\n");
}
