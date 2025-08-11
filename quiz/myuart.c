#include "myuart.h"

uint8_t myuart_tx_buff[UART_TRANSFER_SIZE] = "";
uint8_t myuart_rx_buff[UART_TRANSFER_SIZE] = { 0 };
uart_buffer_config_t myuart_buffer_config = {
	.rx_buffer = myuart_rx_buff,
	.rx_buffer_size = UART_TRANSFER_SIZE
};

void myuart_init(uint8_t bus_id) {
	pin_t rx=0;
	pin_t tx=0;
	switch (bus_id) {
	case 0:rx = 18;
		tx = 17;
		uapi_pin_set_mode(rx, 1);
		uapi_pin_set_mode(tx, 1);
		break;
	case 1:rx = 16;
		tx = 15;
		uapi_pin_set_mode(rx, 1);
		uapi_pin_set_mode(tx, 1);
		break;
	case 2:rx = 7;
		tx = 8;
		uapi_pin_set_mode(rx, 2);
		uapi_pin_set_mode(tx, 2);
		break;
	}
	uart_attr_t attr = {
		.baud_rate = UART_BAUDRATE,
		.data_bits = UART_DATA_BITS,
		.stop_bits = UART_STOP_BITS,
		.parity = UART_PARITY_BIT
	};

	uart_pin_config_t pin_config = {
		.tx_pin = tx,
		.rx_pin = rx,
		.cts_pin = PIN_NONE,
		.rts_pin = PIN_NONE
	};
	uapi_uart_deinit(bus_id);
	uapi_uart_init(bus_id, &pin_config, &attr, NULL, &myuart_buffer_config);
	uapi_uart_register_rx_callback(bus_id, UART_RX_CONDITION_FULL_OR_IDLE, 1, myuart_read_int_handler);
}

void myuart_read_int_handler(const void* buffer, uint16_t length, bool error) {
	unused(error);

	// if (buffer == NULL || length == 0) {
	// 	osal_printk("uart%d int mode transfer illegal data!\r\n", CONFIG_UART_BUS_ID);
	// 	return;
	// }

	// uint8_t* buff = (uint8_t*)buffer;
	// if (memcpy_s(g_app_uart_rx_buff, length, buff, length) != EOK) {
	// 	osal_printk("uart%d int mode data copy fail!\r\n", CONFIG_UART_BUS_ID);
	// 	return;
	// }

	uint8_t* buff = (uint8_t*)buffer;
	memcpy_s(myuart_rx_buff, length, buff, length);
	memcpy_s(myuart_tx_buff, length, buff, length);
	uapi_uart_write(0, myuart_tx_buff, strlen((const char*)myuart_tx_buff), 0);
}
