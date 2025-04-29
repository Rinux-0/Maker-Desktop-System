#pragma once
#ifndef _BBLE_DEF_H_
#define _BBLE_DEF_H_



#include "ddef.h"

#define BLE_UART_BUS (0)

typedef struct {
	u8* value;
	u16 value_len;
} msg_data_t;



#endif // _BBLE_DEF_H_
