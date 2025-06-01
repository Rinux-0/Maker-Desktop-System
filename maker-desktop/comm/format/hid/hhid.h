/*
	供【sle】【uart】使用
*/

#pragma once
#ifndef _HHID_H_
#define _HHID_H_



#include "ddef.h"
#include "hhid_def.h"

void hid_init(void);
const hid_pack_t* hid_set_wp(u8 cmd, u8 data_len, const u8* data, const s16* data_map);
const hid_pack_t* hid_get_wp(void);



#endif // _HHID_H_
