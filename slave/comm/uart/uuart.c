#include "uuart.h"
#include "uuart_def.h"

// #include "ddef.h"
#include "ttool.h"

#include <pinctrl.h>
#include <uart.h>



#define UART_INT_TRANSFER_MODE	// UART_中断传输模式（此宏用于<uart.h>识别）

bool uart_is_inited[3];
static u8 uart_rx_buff[UART_TRANSFER_SIZE];
static uart_buffer_config_t uart_buffer = {
	.rx_buffer = uart_rx_buff,
	.rx_buffer_size = UART_TRANSFER_SIZE
};

static u32 baud_rate[3];
static u8 parity[3];



u8 UART_BUS_ID(u8 bus_id) {
	switch (bus_id) {
	default:		return 3;
	break;case 0:	return UART_0_BUS_ID;
	break;case 1:	return UART_1_BUS_ID;
	break;case 2:	return UART_2_BUS_ID;
	}
}
static u8 UART_TXD_PIN(u8 bus_id) {
	switch (bus_id) {
	default:		return PIN_NONE;
	break;case 0:	return UART_0_TXD_PIN;
	break;case 1:	return UART_1_TXD_PIN;
	break;case 2:	return UART_2_TXD_PIN;
	}
}
static u8 UART_RXD_PIN(u8 bus_id) {
	switch (bus_id) {
	default:		return PIN_NONE;
	break;case 0:	return UART_0_RXD_PIN;
	break;case 1:	return UART_1_RXD_PIN;
	break;case 2:	return UART_2_RXD_PIN;
	}
}



static void uart_init_pin(u8 bus_id) {
	if (bus_id == 2) {
		uapi_pin_set_mode(UART_TXD_PIN(bus_id), PIN_MODE_2);	// TXD模式
		uapi_pin_set_mode(UART_RXD_PIN(bus_id), PIN_MODE_2);	// RXD模式
	} else {
		uapi_pin_set_mode(UART_TXD_PIN(bus_id), PIN_MODE_1);	// TXD模式
		uapi_pin_set_mode(UART_RXD_PIN(bus_id), PIN_MODE_1);	// RXD模式
	}
}


// parity: 0-无校验, 1-奇校验, 2-偶校验
static void uart_init_cfg(u8 bus_id) {
	u32	baud = (baud_rate[bus_id] >= 9600)
		? baud_rate[bus_id]
		: 115200;

	uart_attr_t basic_attr = {
		.baud_rate = baud,
		.data_bits = UART_DATA_BITS,
		.stop_bits = UART_STOP_BITS,
		.parity = parity[bus_id]
	};

	uart_pin_config_t pin_cfg = {
		.tx_pin = UART_TXD_PIN(bus_id),
		.rx_pin = UART_RXD_PIN(bus_id),
		.cts_pin = PIN_NONE,
		.rts_pin = PIN_NONE
	};

	uapi_uart_deinit(UART_BUS_ID(bus_id));
	uapi_uart_init(
		UART_BUS_ID(bus_id),
		&pin_cfg,
		&basic_attr,
		NULL,
		&uart_buffer
	);
}



errcode_t uart_set_r_cb(u8 bus_id, uart_r_cb_t cb) {
	uapi_uart_unregister_rx_callback(UART_BUS_ID(bus_id));

	return uapi_uart_register_rx_callback(
		UART_BUS_ID(bus_id),
		UART_RX_CONDITION_FULL_OR_IDLE,		// 需def UART_INT_TRANSFER_MODE
		1,
		(uart_rx_callback_t)cb
	);
}


void uart_init(u8 bus_id, bool force_init) {
	if (bus_id >= 3 || (uart_is_inited[bus_id] && !force_init))
		return;

	uart_init_pin(bus_id);
	uart_init_cfg(bus_id);

	uart_set_r_cb(bus_id, NULL);

	uart_is_inited[bus_id] = true;

	LOG("baud_rate: %d\n", baud_rate[bus_id]);
}


void uart_oneloop(u8 bus_id) {
	if (bus_id >= 3 || !uart_is_inited[bus_id])
		return;
}


void uart_exit(u8 bus_id) {
	if (bus_id >= 3 || !uart_is_inited[bus_id])
		return;

	uapi_uart_unregister_rx_callback(UART_BUS_ID(bus_id));
	uapi_uart_deinit(UART_BUS_ID(bus_id));

	uart_is_inited[bus_id] = false;
}


void uart_write(u8 bus_id, const u8* data, u32 length) {
	if (bus_id >= 3 || !uart_is_inited[bus_id]) {
		ERROR("uart%d is not inited\n", bus_id);
		return;
	}

	uapi_uart_write(UART_BUS_ID(bus_id), data, length, 0);
}


void uart_set_baud(u8 bus_id, u32 baud) {
	baud_rate[bus_id] = baud;
}


void uart_set_parity(u8 bus_id, u8 _parity) {
	parity[bus_id] = _parity;
}
