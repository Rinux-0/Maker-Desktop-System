#include "nfc.h"

#include "ddef.h"
#include "ttool.h"

#include "ssle.h"
#include "uuart.h"

#include <cmsis_os2.h>



typedef struct {
	const u8 signal;
	u8 sector;
	u8 r_data[3][16];		// [3][*] : 块3是商家信息，无此记录
} nfc_data_t;



// static u8 nfc_r_data[3][16];
static nfc_data_t nfc_data = { .signal = 'n' };
static u8 nfc_cmd_get_data[] = "AT+READ=1,01\r\n";		// 此处必有“\r\n”
static u8 nfc_sector, nfc_block;		// min == 0
static bool gpio_int_flag;
static volatile bool is_wating;


static void nfc_uart_r_int_handler(const void* buffer, u16 length, bool error) {
	unused(error);

	DATA("\n\tsector: %d, block: %d\n\n", nfc_sector, nfc_block);

	// 省略包头包尾判断
	switch (length) {
	default:		ERROR("length error: %d\n", length);
	return;case 4:	DATA("\n\tnfc leaves...\n\n");
	return;case 8:	DATA("\n\tnfc comes...\n\n");
	return;case 24:	DATA("\n\tlength: %d\n\n", length);
	}

	memcpy_s(nfc_data.r_data[nfc_block], 16, (const u8*)buffer + 6, 16);
	// memcpy_s(nfc_r_data[nfc_block], 16, (const u8*)buffer + 6, 16);

	is_wating = false;

	// u8* d = nfc_data.r_data[nfc_block];
	// // u8* d = nfc_r_data[nfc_block];
	// LOG("%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n",
	// 	d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7],
	// 	d[8], d[9], d[10], d[11], d[12], d[13], d[14], d[15]
	// );
}


static bool nfc_write_cmd_get_block(u8 sector, u8 block) {
	u16 addr = sector * 4 + block;

	nfc_cmd_get_data[10] = '0' + addr / 10;
	nfc_cmd_get_data[11] = '0' + addr % 10;

	LOG("\n\t%s\n\n", nfc_cmd_get_data);

	uart_write(UART_BUS_ID(2), nfc_cmd_get_data, sizeof(nfc_cmd_get_data) - 1);
	is_wating = true;

	bool strt = g_time_wait_0s1;
	while (is_wating) {
		tool_sleep_m(1);
		if (strt != g_time_wait_0s1) {
			// DATA("\n\tnfc: error_timeout\n\n");
			return false;
		}
	}

	return true;
}


static void nfc_gpio_r_int_handler(pin_t pin, uintptr_t param) {
	unused(pin);
	unused(param);

	gpio_int_flag = true;
}


static void nfc_write_get_req(void) {
	if (gpio_int_flag == false)
		return;

	// 请求 / 处理 nfc 数据
	for (u8 i = 0; i < 16; i++) {
		nfc_data.sector = nfc_sector = i;
		for (u8 j = 0; j < 3; j++) {		// j == 3 : 商家信息，跳过
			u8 time_try = 0;
			nfc_block = j;

			// nfc 已离开
			if (0 == tool_pin_gpio_get_val(2)) {
				gpio_int_flag = false;
				return;
			}

			// 若发送失败，则重试
			while (!nfc_write_cmd_get_block(nfc_sector, nfc_block)) {
				tool_sleep_m(1);
				// 无法成功发送，退出处理
				if (time_try++ > 8) {
					ERROR("\n\tnfc: error_send_fail\n\n");
					gpio_int_flag = false;
					return;
				}
			}
		}

		sle_write(pc, (u8*)&nfc_data, sizeof(nfc_data));
		// sle_write(pc, (u8*)&nfc_r_data, sizeof(nfc_r_data));
	}

	gpio_int_flag = false;
}


static void nfc_init(void) {
	// GPIO中断 设置
	uapi_pin_set_mode(2, 0);
	uapi_gpio_set_dir(2, GPIO_DIRECTION_INPUT);
	uapi_gpio_register_isr_func(2, 1, nfc_gpio_r_int_handler);

	// GPIO中断 使能
	uapi_gpio_enable_interrupt(2);

	// uart_set_baud(UART_BUS_ID(2), 9600);
	// uart_init(UART_BUS_ID(2), true);

	uart_set_r_cb(UART_BUS_ID(2), nfc_uart_r_int_handler);
}


static void nfc_oneloop(void) {
	tool_sleep_m(1);

	nfc_write_get_req();
}


static void nfc_exit(void) {}



static void* nfc(const c8* arg) {
	unused(arg);

	nfc_init();
	while (1)
		nfc_oneloop();
	nfc_exit();

	return NULL;
}


void nfc_entry(void) {
	osThreadAttr_t attr = {
		.name = "nfc",
		.attr_bits = 0U,
		.cb_mem = NULL,
		.cb_size = 0U,
		.stack_mem = NULL,
		.stack_size = 0x1000,
		.priority = (osPriority_t)17,
		// .tz_module	= 0U,
		// .reserved	= 0U
	};

	if (NULL == osThreadNew((osThreadFunc_t)nfc, NULL, &attr)) {
		ERROR("Failed to create nfc sub_thread");
	}
}
