#ifndef _HHID_CH9329_H_
#define _HHID_CH9329_H_



#include "ddef.h"
#include "hhid_ch9329_def.h"

void hid_ch9329_init(void);
void hid_ch9329_set_wp(u8 cmd, u8 data_len, const u8* data, const s16* data_map);
const hid_ch9329_pack* hid_ch9329_get_wp(void);



#endif // _HHID_CH9329_H_
