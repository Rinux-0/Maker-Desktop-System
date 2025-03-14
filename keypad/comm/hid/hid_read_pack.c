/// @attention 该文件仅保留必要函数即可，最开始可完全放弃



#include "hid.h"

#include "../../util/util_def.h"
#include "hid_cmd_def.h"



static HID_Pack hid_r_pack;	// 读包



/// @todo 待完成
/// @brief 获取芯片版本等信息 -CMD_GET_INFO
static void hid_r_pack_destruct_cmd0x01(void) {

}


/// @todo 待完成
/// @brief 读取USB键盘普通数据 -CMD_SEND_KB_GENERAL_DATA
static void hid_r_pack_destruct_cmd0x02(u8* hid_data, u8 hid_data_len) {

}


/// @todo 待完成
/// @brief 读取USB相对鼠标数据 -CMD_SEND_MS_REL_DATA
static void hid_r_pack_destruct_cmd0x05(u8* hid_data, u8 hid_data_len) {

}


/// @todo 待完成
/// @brief 读取USB自定义HID设备数据 -CMD_SEND_MY_HID_DATA
static void hid_r_pack_destruct_cmd0x06(u8* hid_data, u8 hid_data_len) {

}


/// @todo 待完成
/// @note 无对应的请求包
/// @brief CMD_READ_MY_HID_DATA
static void hid_r_pack_destruct_cmd0x87(u8* hid_data, u8 hid_data_len) {

}


/// @todo 待完成
/// @brief 复位芯片 -CMD_RESET
static void hid_r_pack_destruct_cmd0x0F(void) {

}


/// @todo 待完成
HID_Pack hid_read_pack_destruct(u8 cmd) {
	return (HID_Pack) { 0 };
}


/// @todo 待完成（可忽略）
// 其他hid错误应答包的处理
