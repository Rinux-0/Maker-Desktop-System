#pragma once
#ifndef _SSLE_H_
#define _SSLE_H_



#include "ddef.h"
#include "ssle_def.h"
#include <errcode.h>

errcode_t sle_set_r_cb(sle_r_cb_t cb);
void sle_uart_r_int_handler(const void* buffer, u16 length, bool error);
void sle_write(u8 conn_id, const u8* data, u32 length);

void sle_init(void);
void sle_oneloop(void);
void sle_exit(void);



#endif // _SSLE_H_
