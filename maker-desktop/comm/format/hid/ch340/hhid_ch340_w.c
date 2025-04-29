#include "hhid_ch340.h"
#include "hhid_ch340_def.h"
#include "hhid_def.h"

#include "ddef.h"



hid_ch340_pack hid_ch340_wp;



/// @todo 发送描述符
void hid_init(void) {}


/// @todo 设置数据包
void hid_set_wp(u8 cmd, u8 data_len, const u8* data, const s16* data_map) {}


const hid_ch340_pack* hid_get_wp(void) {
	return &hid_ch340_wp;
}
