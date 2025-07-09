#pragma once
#ifndef _SSLE_H_
#define _SSLE_H_
/*
	若春在局部通信方向: sle_client <-> sle_server
	则强制要求: data[0] == '@' && ( data[1] 在 sle_target_t 范围内 )
*/



#include "ddef.h"
#include "ssle_def.h"

/** @param[in] target_id 发送对象的 规定index
 *	0-pc
 *	1-deskaide
 *	2-dynamic
 *	3-health
 *	4-keyboard
 *	5-keypad
 *	6-receiver
 *	7-keytest
 *	8-tmptest
**/
void sle_write(sle_target_t target_id, const u8* data, u32 length);
errcode_t sle_set_r_cb(sle_r_cb_t cb);

void sle_init(void);
void sle_oneloop(void);
void sle_exit(void);



#endif // _SSLE_H_
