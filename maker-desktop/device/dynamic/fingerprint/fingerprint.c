#include "fingerprint.h"

#include "ddef.h"
#include "ttool.h"

#include "../dynamic_def.h"
#include "ssle.h"
#include "uuart.h"



static u8 str_fingerprint[12];
static fingerprint_t fingerprint_r_data;
static u8 fingerprint_cmd_get_data[] = {
	0xEF, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x08,
	0x32, 0x02, 0xFF, 0xFF, 0x00, 0x01, 0x02, 0x3C, '\0'
};
static bool is_wating;



static void fingerprint_uart_r_int_handler(const void* buffer, u16 length, bool error) {
	unused(error);

	// 省略包头包尾判断
	if (length != 17) {
		ERROR("length error: %d\n", length);
		return;
	}

	const u8* buff = (const u8*)buffer;

	if (buff[9] == 0x00 && buff[10] == 0x05) {
		fingerprint_r_data.id = buff[11] * 0x100 + buff[12] * 0x1;
		fingerprint_r_data.grade = buff[12] * 0x100 + buff[12] * 0x1;
		fingerprint_r_data.is_valid = true;
		sprintf((c8*)str_fingerprint, "fi%02d,fg%04d", fingerprint_r_data.id, fingerprint_r_data.grade);
		sle_write(receiver, (u8*)str_fingerprint, sizeof(str_fingerprint) - 1);

		DATA("\n\tid: %d, grade: %d, is_valid: %d\n\n",
			 fingerprint_r_data.id, fingerprint_r_data.grade, fingerprint_r_data.is_valid
		);
	} else if (buff[9] == 0x09 && buff[10] == 0x05) {
		fingerprint_r_data.is_valid = false;
		sle_write(receiver, (u8*)"error", sizeof("error") - 1);

		ERROR("recognize error\n");
	}

	is_wating = false;
}


static void fingerprint_gpio_r_int_handler(pin_t pin, uintptr_t param) {
	unused(pin);
	unused(param);

	/// @todo: 请求 / 处理 fingerprint 数据
	uart_write(UART_BUS_ID(0), fingerprint_cmd_get_data, sizeof(fingerprint_cmd_get_data) - 1);
	is_wating = true;

	while (is_wating)
		tool_delay_m(10);
}


void fingerprint_init(void) {
	// GPIO中断 设置
	uapi_pin_set_mode(0, 0);
	uapi_gpio_set_dir(0, GPIO_DIRECTION_INPUT);
	uapi_gpio_register_isr_func(0, 1, fingerprint_gpio_r_int_handler);

	// GPIO中断 使能
	uapi_gpio_enable_interrupt(0);

	// UART
	uart_set_baud(UART_BUS_ID(0), 57600);
	uart_init(UART_BUS_ID(0), true);
	uart_set_r_cb(UART_BUS_ID(0), fingerprint_uart_r_int_handler);
}


void fingerprint_oneloop(void) {}


void fingerprint_exit(void) {}
