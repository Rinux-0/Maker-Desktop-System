#ifndef _MYUART_H_
#define _MYUART_H_

#include "common_def.h"
#include "osal_debug.h"
#include "cmsis_os2.h"
#include "app_init.h"
#include "pinctrl.h"
#include <unistd.h>
#include "uart.h"
#include "main.h"
#include "stdio.h"

#define UART_BAUDRATE                      115200
#define UART_DATA_BITS                     3
#define UART_STOP_BITS                     1
#define UART_PARITY_BIT                    0
#define UART_TRANSFER_SIZE                 512
#define CONFIG_UART_INT_WAIT_MS            5

extern uint8_t myuart_tx_buff[UART_TRANSFER_SIZE];
extern uint8_t myuart_rx_buff[UART_TRANSFER_SIZE];
extern uart_buffer_config_t myuart_buffer_config;

void myuart_init(uint8_t bus_id);
void myuart_read_int_handler(const void* buffer, uint16_t length, bool error);

#endif
