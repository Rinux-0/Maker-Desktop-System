#include "hhid_ch9329.h"
#include "hhid_ch9329_def.h"
#include "hhid_def.h"

#include "ddef.h"



static hid_ch9329_pack hid_ch9329_wp;	// 写包



void hid_ch9329_init(void) {}


/// @brief 获取芯片版本等信息 -HID_CH9329_CMD_GET_INFO
static void hid_ch9329_set_wp_cmd0x01(void) {
	hid_ch9329_wp.length	= 0;
	hid_ch9329_wp.sum		= 3;	// 基础值
}


/// @todo Fn键
/// @brief 发送USB键盘普通数据 -HID_CH9329_CMD_SEND_KB_GENERAL_DATA
static void hid_ch9329_set_wp_cmd0x02(u8 data_len, const u8* data, const s16* data_map) {
	hid_ch9329_wp.length	=  8;
	hid_ch9329_wp.sum		= 12;	// 基础值

	hid_ch9329_wp.data[0] = 0;		// 修饰键：预先置0
	hid_ch9329_wp.data[1] = 0;		// CH9329要求置0

	u8 num_normal_key = 0;

	for (u8 i=0; i<data_len; i++)				// 每构造包，逐寄存器
		for (u8 j=0; j<8; j++)					// 每寄存器，逐bit
			if (data[i] & (1<<j)) {				// 按下
				if (data_map[i*8 + j] < 0)		// Fn / 修饰键
					(data_map[i*8 + j] == Fn)
					? (0)						/// @todo
					: (hid_ch9329_wp.data[0] |= 1 << (~data_map[i*8+j] - 1));
				else if (num_normal_key < 6)	// 普通键
					hid_ch9329_wp.data[2+num_normal_key++] = data_map[i*8+j];
			}

	for (; num_normal_key<6; num_normal_key++)
		hid_ch9329_wp.data[2+num_normal_key] = 0;	// 普通键：余位->置0
}


/// @todo 待完成
/// @brief 发送USB相对鼠标数据 -HID_CH9329_CMD_SEND_MS_REL_DATA
static void hid_ch9329_set_wp_cmd0x05(u8 data_len, const u8* data, const s16* data_map) {

}


/// @todo 待完成
/// @brief 发送USB自定义HID设备数据 -HID_CH9329_CMD_SEND_MY_HID_DATA
static void hid_ch9329_set_wp_cmd0x06(u8 data_len, const u8* data, const s16* data_map) {

}


/// @note 无此函数，仅作为cmd0x87的对立参考
/// @brief HID_CH9329_CMD_READ_MY_HID_DATA (0x87)
// static void hid_ch9329_r_pack_make_cmd0x07(void) {}


/// @todo 待完成
/// @brief 复位芯片 -HID_CH9329_CMD_RESET
static void hid_ch9329_set_wp_cmd0x0F(void) {

}


/// @todo 待完善
void hid_ch9329_set_wp(u8 cmd, u8 data_len, const u8* data, const s16* data_map) {
	if (cmd < 0x01 || cmd > 0x3F)
		return;

	// head
	hid_ch9329_wp.head[0] = 0x57;
	hid_ch9329_wp.head[1] = 0xAB;

	// addr
	hid_ch9329_wp.addr = 0x00;

	// cmd
	hid_ch9329_wp.cmd = cmd;

	// length & data
	switch (cmd) {
	default:
	break;case HID_CH9329_CMD_GET_INFO:
		hid_ch9329_set_wp_cmd0x01();
	break;case HID_CH9329_CMD_SEND_KB_GENERAL_DATA:
		hid_ch9329_set_wp_cmd0x02(data_len, data, data_map);
	break;case HID_CH9329_CMD_SEND_MS_REL_DATA:
		hid_ch9329_set_wp_cmd0x05(data_len, data, data_map);
	break;case HID_CH9329_CMD_SEND_MY_HID_DATA:
		hid_ch9329_set_wp_cmd0x06(data_len, data, data_map);
	break;case HID_CH9329_CMD_RESET:
		hid_ch9329_set_wp_cmd0x0F();
	}

	// sum
	for (u8 i=0; i<hid_ch9329_wp.length; i++)
		hid_ch9329_wp.sum += hid_ch9329_wp.data[i];
	hid_ch9329_wp.data[hid_ch9329_wp.length] = hid_ch9329_wp.sum;	// sum --> data末尾
}


const hid_ch9329_pack* hid_ch9329_get_wp(void) {
	return &hid_ch9329_wp;
}
