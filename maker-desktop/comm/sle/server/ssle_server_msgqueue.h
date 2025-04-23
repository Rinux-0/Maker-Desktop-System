#pragma once
#ifndef _SSLE_SERVER_H_
#define _SSLE_SERVER_H_



#include "ddef.h"

void sle_server_msgqueue_create(void);
void sle_server_msgqueue_delete(void);
void sle_server_msgqueue_write(u8* buffer_addr, u16 buffer_size);
s32 sle_server_msgqueue_receive(u8* buffer_addr, u32* buffer_size);



#endif // _SSLE_SERVER_H_
