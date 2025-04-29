#pragma once
#ifndef _HHID_CH340_H_
#define _HHID_CH340_H_



#include "ddef.h"
#include "hhid_ch340_def.h"

void hid_init(void);
void hid_set_wp(u8 cmd, u8 data_len, const u8* data, const s16* data_map);
const hid_ch340_pack* hid_get_wp(void);



#endif // _HHID_CH340_H_
