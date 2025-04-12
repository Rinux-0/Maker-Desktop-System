#ifndef _COMM_HID_H_
#define _COMM_HID_H_



#include "comm_hid_def.h"
#include "util_def.h"

void hid_wp_construct(u8 cmd, u8 data_len, const u8* data, const s16* data_map);
const hid_pack* hid_get_wp(void);



#endif // _COMM_HID_H_
