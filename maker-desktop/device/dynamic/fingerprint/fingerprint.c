#include "fingerprint.h"

#include "ddef.h"
#include "ttool.h"

#include "../dynamic_def.h"
#include "ssle.h"
#include "uuart.h"

#include <cmsis_os2.h>



#define FINGERPRINT_GPIO_INT_PIN 1



static u8 str_fingerprint[4];
static fingerprint_t fingerprint_r_data;
static u8 fingerprint_cmd_get_data[] = {
	0xEF, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x08,
	0x32, 0x02, 0xFF, 0xFF, 0x00, 0x01, 0x02, 0x3C, '\0'
};
static volatile bool is_wating;
static bool gpio_int_flag;



static void fingerprint_uart_r_int_handler(const void* buffer, u16 length, bool error) {
	unused(error);

	LOG("\n\tlen: %d\n\n", length);

	// 省略包头包尾判断
	if (length != 17) {
		ERROR("length error: %d\n", length);
		return;
	}

	const u8* buff = (const u8*)buffer;

	if (buff[9] == 0x00 && buff[10] == 0x05) {
		fingerprint_r_data.id = buff[11] * 0x100 + buff[12] * 0x1;
		fingerprint_r_data.grade = buff[13] * 0x100 + buff[14] * 0x1;
		fingerprint_r_data.is_valid = true;
		sprintf((c8*)str_fingerprint, "f%02d", fingerprint_r_data.id);
		sle_write(pc, (u8*)str_fingerprint, sizeof(str_fingerprint) - 1);

		DATA("\n\tid: %d, grade: %d, is_valid: %d\n\n",
			fingerprint_r_data.id, fingerprint_r_data.grade, fingerprint_r_data.is_valid
		);
	} else if (buff[9] == 0x09 && buff[10] == 0x05) {
		fingerprint_r_data.is_valid = false;
		sle_write(pc, (u8*)"fer", sizeof("fer") - 1);

		ERROR("recognize error\n");
	}

	is_wating = false;
}


static void fingerprint_gpio_r_int_handler(pin_t pin, uintptr_t param) {
	unused(pin);
	unused(param);

	gpio_int_flag = true;
	// LOG("");
}


static void fingerprint_write_get_req(void) {
	// if (gpio_int_flag == false)
	// 	return;

	// LOG("");

	uart_write(UART_BUS_ID(0), fingerprint_cmd_get_data, sizeof(fingerprint_cmd_get_data) - 1);
	is_wating = true;

	bool strt = g_time_wait_0s1;
	while (is_wating) {
		tool_sleep_m(1);
		if (strt != g_time_wait_0s1) {
			// DATA("\n\tfingerprint: error_timeout\n\n");
			break;
		}
	}

	gpio_int_flag = false;
}


static void fingerprint_init(void) {
	// GPIO中断 设置
	uapi_pin_set_mode(FINGERPRINT_GPIO_INT_PIN, 0);
	uapi_gpio_set_dir(FINGERPRINT_GPIO_INT_PIN, GPIO_DIRECTION_INPUT);
	uapi_gpio_register_isr_func(FINGERPRINT_GPIO_INT_PIN, GPIO_INTERRUPT_RISING_EDGE, fingerprint_gpio_r_int_handler);

	// GPIO中断 使能
	uapi_gpio_enable_interrupt(FINGERPRINT_GPIO_INT_PIN);

	// UART
	// uart_set_baud(UART_BUS_ID(0), 57600);
	// uart_init(UART_BUS_ID(0), true);

	uart_set_r_cb(UART_BUS_ID(0), fingerprint_uart_r_int_handler);
}


static void fingerprint_oneloop(void) {
	tool_sleep_m(1);

	if (gpio_int_flag == true)
		fingerprint_write_get_req();
}


static void fingerprint_exit(void) {}



static void* fingerprint(const c8* arg) {
	unused(arg);

	fingerprint_init();
	while (1)
		fingerprint_oneloop();
	fingerprint_exit();

	return NULL;
}


void fingerprint_entry(void) {
	osThreadAttr_t attr = {
		.name = "fingerprint",
		.attr_bits = 0U,
		.cb_mem = NULL,
		.cb_size = 0U,
		.stack_mem = NULL,
		.stack_size = 0x1000,
		.priority = (osPriority_t)17,
		// .tz_module	= 0U,
		// .reserved	= 0U
	};

	if (NULL == osThreadNew((osThreadFunc_t)fingerprint, NULL, &attr)) {
		ERROR("Failed to create fingerprint sub_thread");
	}
}
