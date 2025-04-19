#include "uuart.h"
#include "uuart_def.h"

#include "def.h"
#include "ssle_def.h"
#include "tool.h"

#include <errcode.h>
#include <pinctrl.h>
#include <uart.h>



static u8 uart_rx_buff[UART_TRANSFER_SIZE] = {};
static uart_buffer_config_t uart_buffer = {
	.rx_buffer = uart_rx_buff,
	.rx_buffer_size = UART_TRANSFER_SIZE
};



static void uart_init_pin(void) {
	uapi_pin_set_mode(UART_TXD_PIN, PIN_MODE_1);	// TXD模式
	uapi_pin_set_mode(UART_RXD_PIN, PIN_MODE_1);	// RXD模式
}


static void uart_init_cfg(void) {
	uart_attr_t basic_attr = {
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
		&basic_attr,
		NULL,
		&uart_buffer
	);
}


// /// @todo 待完善（可忽略）
// /// @brief read中断回调
// static void uart_r_int_handler(const void* buffer, u16 length, bool error) {
// 	unused(error);

// }


errcode_t uart_set_r_cb(uart_r_cb_t cb) {
	uapi_uart_unregister_rx_callback(UART_BUS_ID);
	return uapi_uart_register_rx_callback(
		UART_BUS_ID,
		UART_RX_CONDITION_FULL_OR_IDLE,		// 需def UART_INT_TRANSFER_MODE
		1,
		(uart_rx_callback_t)cb
	);
}


void uart_init(void) {
	uart_init_pin();
	uart_init_cfg();

	// uart_set_r_cb(uart_r_int_handler);
}


void uart_exit(void) {
	uapi_uart_unregister_rx_callback(UART_BUS_ID);
	uapi_uart_deinit(UART_BUS_ID);
}


void uart_write(const u8* data, u32 length) {
	uapi_uart_write(UART_BUS_ID, data, length, 0);
}


void uart_sle_r_int_handler(u8 cs_id, u16 conn_id, ssle_ssap_value_t* read_cb_para, errcode_t status) {
	/// @todo
	LOG("uart_sle_r_int_handler: cs_id=%d, conn_id=%d, status=%d\n", cs_id, conn_id, status);

	#if defined(CONFIG_COMM_SLE_CLIENT)
	uart_write(read_cb_para->data, read_cb_para->data_len);
	#elif defined(CONFIG_COMM_SLE_SERVER)
	uart_write(read_cb_para->value, read_cb_para->length);
	#endif
}
