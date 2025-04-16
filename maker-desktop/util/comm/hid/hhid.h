#ifndef _HHID_H_
#define _HHID_H_



#include "hhid_def.h"
#include "def.h"

void hid_wp_construct(u8 cmd, u8 data_len, const u8* data, const s16* data_map);
const hid_pack* hid_get_wp(void);



#endif // _HHID_H_
