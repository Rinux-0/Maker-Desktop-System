#include "sound.h"

#include "ddef.h"
#include "ttool.h"

#include "core/snd_core.h"
#include "ssle.h"
#include "uuart.h"

#include <cmsis_os2.h>



static void sound_uart_r_int_handler(const void* buffer, u16 length, bool error) {
	unused(error);

	snd_cmd_entry((c8*)buffer, length);
}


static void sound_sle_r_int_handler(u8 cs_id, u16 conn_id, ssle_ssap_value_t* read_cb_para, errcode_t status) {
	unused(cs_id);
	unused(conn_id);
	unused(status);

	snd_cmd_entry((c8*)read_cb_para->value, read_cb_para->length);
}


static void sound_init(void) {
	sle_set_r_cb(sound_sle_r_int_handler);
	uart_set_r_cb(UART_BUS_ID(2), sound_uart_r_int_handler);

	snd_core_init_pin();
}


static void sound_oneloop(void) {
	tool_delay_m(1);
}


static void sound_exit(void) {
	// LOG("sound_exit\n");
}



static void* sound(const c8* arg) {
	unused(arg);

	sound_init();
	while (1)
		sound_oneloop();
	sound_exit();

	return NULL;
}


void sound_entry(void) {
	osThreadAttr_t attr = {
		.name = "sound",
		.attr_bits = 0U,
		.cb_mem = NULL,
		.cb_size = 0U,
		.stack_mem = NULL,
		.stack_size = 0x1000,
		.priority = (osPriority_t)17,
		// .tz_module	= 0U,
		// .reserved	= 0U
	};

	if (NULL == osThreadNew((osThreadFunc_t)sound, NULL, &attr)) {
		ERROR("Failed to create sound sub_thread");
	}
}
