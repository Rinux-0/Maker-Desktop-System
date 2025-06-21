#pragma once
#ifndef _HHID_XXX_H_
#define _HHID_XXX_H_



#include "ddef.h"
#include "hhid_xxx_def.h"

void hid_init(void);
void hid_set_wp(u8 cmd, u8 data_len, const u8* data, const s16* data_map);
const hid_xxx_pack_t* hid_get_wp(void);



#endif // _HHID_XXX_H_
