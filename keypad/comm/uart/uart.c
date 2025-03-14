#include <pinctrl.h>
#include <uart.h>
#include "uart.h"

#include "../../comm/hid/hid.h"
#include "../../util/util_def.h"
#include "uart_cfg_def.h"



static u8 uart_int_rx_flag = 0;
static u8 uart_rx_buff[UART_TRANSFER_SIZE] = {};
static uart_buffer_config_t uart_buffer = {
	.rx_buffer = uart_rx_buff,
	.rx_buffer_size = UART_TRANSFER_SIZE
};


static void uart_pin_init(void) {
	uapi_pin_set_mode(UART_TXD_PIN, PIN_MODE_1);	// TXD模式
	uapi_pin_set_mode(UART_RXD_PIN, PIN_MODE_1);	// RXD模式
}


static void uart_cfg_init(void) {
	uart_attr_t attr = {
		.baud_rate	= UART_BAUDRATE,
		.data_bits	= UART_DATA_BITS,
		.stop_bits	= UART_STOP_BITS,
		.parity		= UART_PARITY_BIT
	};

	uart_pin_config_t pin_cfg = {
		.tx_pin  = UART_TXD_PIN,
		.rx_pin  = UART_RXD_PIN,
		.cts_pin = PIN_NONE,
		.rts_pin = PIN_NONE
	};

	uapi_uart_deinit(UART_BUS_ID);
	uapi_uart_init(
		UART_BUS_ID,
		&pin_cfg,
		&attr,
		NULL,
		&uart_buffer
	);
}


/// @attention 需配合处理来自hid的响应包信息
/// @todo 待完善
/// @brief read中断回调的附加处理
static void uart_read_int_handler(const void* buffer, u16 length, bool error) {
	unused(error);

}


/// @todo 待完善（可忽略）
/// @brief write中断回调的附加处理
static void uart_write_int_handler(const void* buffer, u32 length, const void* params) {
	unused(params);

}


void uart_init(void) {
	uart_pin_init();
	uart_cfg_init();

	// 注册read中断回调
	uapi_uart_register_rx_callback(
		UART_BUS_ID,
		UART_RX_CONDITION_FULL_OR_IDLE,		// 需def UART_INT_TRANSFER_MODE
		1,
		uart_read_int_handler
	);
}


void uart_send_hid_pack(void) {
	uapi_uart_write(
		UART_BUS_ID,
		(const u8*)get_hid_w_pack(),
		get_hid_w_pack()->length + 6,
		0
	);
}
