/// @attention 初期，该文件仅完成保留必要函数即可



#include "hid.h"

#include "../../keypad/keypad.h"
#include "../../util/util_def.h"
#include "hid_cmd_def.h"




static HID_Pack hid_w_pack;	// 写包



/// @brief 获取芯片版本等信息 -CMD_GET_INFO
static void hid_w_pack_construct_cmd0x01(void) {
	hid_w_pack.length	= 0;
	hid_w_pack.sum		= 3;	// 基础值
}


/// @todo 待完成
/// @warning 此处为硬编码，仅供小键盘临时参考
/// @brief 发送USB键盘普通数据 -CMD_SEND_KB_GENERAL_DATA
static void hid_w_pack_construct_cmd0x02(const u8* hid_data, u8 hid_data_len) {
	hid_w_pack.length	=  8;
	hid_w_pack.sum		= 12;	// 基础值

	static u8 num_bit;
	num_bit = 2;

	for (u8 i=0; i<hid_data_len && num_bit<8; i++)	// 每构造包，逐寄存器
		for (u8 j=0; j<8 && num_bit<=8; j++)				// 每寄存器，逐bit
			if (hid_data[i] & (1<<j))
				hid_w_pack.data_sum[num_bit++] = 4 + i*8 + j;	// [ A ~ P ] (暂未考虑修饰键)

	for (; num_bit<8; num_bit++)
		hid_w_pack.data_sum[num_bit] = 0;	// 余位置0
}


/// @todo 待完成
/// @brief 发送USB相对鼠标数据 -CMD_SEND_MS_REL_DATA
static void hid_w_pack_construct_cmd0x05(const u8* hid_data, u8 hid_data_len) {

}


/// @todo 待完成
/// @brief 发送USB自定义HID设备数据 -CMD_SEND_MY_HID_DATA
static void hid_w_pack_construct_cmd0x06(const u8* hid_data, u8 hid_data_len) {

}


/// @note 无此函数，仅作为cmd0x87的对立参考
/// @brief CMD_READ_MY_HID_DATA
// static void hid_r_pack_make_cmd0x07(void) {}


/// @todo 待完成
/// @brief 复位芯片 -CMD_RESET
static void hid_w_pack_construct_cmd0x0F(void) {

}


/// @todo 待完善
/// @param hid_data 构造w包所需的数据
/// @param hid_data_len 构造w包所需数据的长度
HID_Pack hid_write_pack_construct(u8 cmd) {
	if (cmd < 0x01 || cmd > 0x3F)
		return (HID_Pack) { 0 };

	// head
	hid_w_pack.head[0] = 0x57;
	hid_w_pack.head[1] = 0xAB;

	// addr
	hid_w_pack.addr = 0x00;

	// cmd
	hid_w_pack.cmd = cmd;

	// length & data_sum(data)
	switch (cmd) {
	default:
	break;case CMD_GET_INFO:
		hid_w_pack_construct_cmd0x01();
	break;case CMD_SEND_KB_GENERAL_DATA:
		hid_w_pack_construct_cmd0x02(get_keypad_now(), NUM_REGISTER);
	break;case CMD_SEND_MS_REL_DATA:
		hid_w_pack_construct_cmd0x05(NULL, 0);
	break;case CMD_SEND_MY_HID_DATA:
		hid_w_pack_construct_cmd0x06(NULL, 0);
	break;case CMD_RESET:
		hid_w_pack_construct_cmd0x0F();
	}

	// sum
	for (u8 i=0; i<hid_w_pack.length; i++)
		hid_w_pack.sum += hid_w_pack.data_sum[i];

	// data_sum(sum)
	/// @note 因发送包的需要，包.data_sum[length] = 包.sum
	///		(可能越界，但为简单，不做处理)
	hid_w_pack.data_sum[hid_w_pack.length] = hid_w_pack.sum;

	return hid_w_pack;
}


/// @note 暂无get_hid_r_pack
const HID_Pack* get_hid_w_pack(void) {
	return &hid_w_pack;
}
