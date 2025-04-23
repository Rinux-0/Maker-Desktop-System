#include "hhid_custom.h"
#include "hhid_custom_def.h"
#include "hhid_def.h"

#include "ddef.h"



hid_custom_pack hid_custom_wp;



/// @todo 发送描述符
void hid_init(void) {}


/// @todo 设置数据包
void hid_set_wp(u8 cmd, u8 data_len, const u8* data, const s16* data_map) {}


const hid_custom_pack* hid_get_wp(void) {
	return &hid_custom_wp;
}
