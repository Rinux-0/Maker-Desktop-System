#include "nfc.h"

#include "ddef.h"
#include "ttool.h"

#include "ssle.h"
#include "uuart.h"



// [0][0] : 开头是商家信息，无用
// [*][3] : 块4是商家信息，无此记录
static u8 nfc_r_data[16][3][16];
static u8 nfc_cmd_get_data[] = "AT+READ=1,01\r\n";		// 此处必有“\r\n”
static s8 nfc_sector = -1, nfc_block = -1;		// min == 0 (-1表示无效)
static bool is_wating;


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

	memcpy_s(nfc_r_data[nfc_sector][nfc_block], 16, (const u8*)buffer + 6, 16);

	is_wating = false;

	// u8* d = nfc_r_data[nfc_sector][nfc_block];
	// LOG("%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n",
	// 	d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7],
	// 	d[8], d[9], d[10], d[11], d[12], d[13], d[14], d[15]
	// );
}


static void nfc_write_cmd_get_block(u8 sector, u8 block) {
	u16 addr = sector * 4 + block;

	nfc_cmd_get_data[10] = '0' + addr / 10;
	nfc_cmd_get_data[11] = '0' + addr % 10;

	uart_write(UART_BUS_ID(2), nfc_cmd_get_data, sizeof(nfc_cmd_get_data) - 1);
	is_wating = true;

	while (is_wating)
		tool_delay_m(10);
}



static void nfc_gpio_r_int_handler(pin_t pin, uintptr_t param) {
	unused(pin);
	unused(param);

	nfc_sector = nfc_block = 0;

	// 请求 / 处理 nfc 数据
	for (u8 i = 0; i < 16; i++) {
		nfc_sector = i;
		for (u8 j = 0; j < 3; j++) {		// j == 3 : 商家信息，跳过
			nfc_block = j;
			nfc_write_cmd_get_block(nfc_sector, nfc_block);
		}
		sle_write(receiver, (u8*)nfc_r_data[nfc_sector], sizeof(nfc_r_data[nfc_sector]));
	}

	nfc_sector = nfc_block = -1;
}


void nfc_init(void) {
	// GPIO中断 设置
	uapi_pin_set_mode(1, 0);
	uapi_gpio_set_dir(1, GPIO_DIRECTION_INPUT);
	uapi_gpio_register_isr_func(1, 1, nfc_gpio_r_int_handler);

	// GPIO中断 使能
	uapi_gpio_enable_interrupt(1);

	uart_set_r_cb(UART_BUS_ID(2), nfc_uart_r_int_handler);
}


void nfc_oneloop(void) {}


void nfc_exit(void) {}
