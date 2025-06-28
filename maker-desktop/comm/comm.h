#pragma once
#ifndef _COMM_H_
#define _COMM_H_



#include "ddef.h"
#include "comm_def.h"

extern comm_way_t comm_way;
extern const bool spi_need_inited[2];
extern const bool uart_need_inited[3];

void comm_init(void);
void comm_oneloop(void);
void comm_exit(void);

#if defined(CONFIG_COMM_UART) && defined(CONFIG_COMM_SLE)
#	include "ssle.h"
#	include "uuart.h"
void sle_r_int_uart_handler(u8 cs_id, u16 conn_id, ssle_ssap_value_t* read_cb_para, errcode_t status);
void uart_r_int_sle_handler(const void* buffer, u16 length, bool error);
#endif



#endif // _COMM_H_
