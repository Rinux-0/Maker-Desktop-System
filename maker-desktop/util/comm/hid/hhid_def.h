#pragma once
#ifndef _HHID_DEF_H_
#define _HHID_DEF_H_



#include "def.h"

/** 命令码-状态值 **/
#define CMD_SUCCESS		(0x00) // 命令执行成功
#define CMD_ERR_TIMEOUT (0xE1) // 串口接收一个字节超时
#define CMD_ERR_HEAD	(0xE2) // 串口接收包头字节出错
#define CMD_ERR_CMD		(0xE3) // 串口接收命令码错误
#define CMD_ERR_SUM		(0xE4) // 累加和检验值不匹配
#define CMD_ERR_PARA	(0xE5) // 参数错误
#define CMD_ERR_OPERATE (0xE6) // 帧正常，执行失败

/** 命令码 **/
#define CMD_GET_INFO				(0x01) // 获取芯片版本等信息
#define CMD_SEND_KB_GENERAL_DATA	(0x02) // 发送USB键盘普通数据
#define CMD_SEND_KB_MEDIA_DATA		(0x03) // 发送USB键盘多媒体数据
#define CMD_SEND_MS_ABS_DATA		(0x04) // 发送USB绝对鼠标数据
#define CMD_SEND_MS_REL_DATA		(0x05) // 发送USB相对鼠标数据
#define CMD_SEND_MY_HID_DATA		(0x06) // 发送USB自定义HID设备数据
#define CMD_READ_MY_HID_DATA		(0x87) // 读取USB自定义HID设备数据（无回复）
#define CMD_GET_PARA_CFG			(0x08) // 获取参数配置
#define CMD_SET_PARA_CFG			(0x09) // 设置参数配置
#define CMD_GET_USB_STRING			(0x0A) // 获取USB字符串描述符配置
#define CMD_SET_USB_STRING			(0x0B) // 设置USB字符串描述符配置
#define CMD_SET_DEFAULT_CFG			(0x00) // 恢复出厂默认配置
#define CMD_RESET					(0x0F) // 复位芯片

#pragma pack(1)
/// @param sum = 无data时的值 + data （手册计算法则有误，需参考不同cmd具体包的例子）
typedef struct {
	u8 head[2];
	u8 addr;
	u8 cmd;
	u8 length;		// length <= 64
	u8 data[64];	// data[length] = sum (可能越界，但更简便)
	u8 sum;			// 包校验和
}hid_pack;

typedef enum {
	Ctrl_L = ~1, Shift_L = ~2, Alt_L = ~3, GUI_L = ~4,
	Ctrl_R = ~5, Shift_R = ~6, Alt_R = ~7, GUI_R = ~8,

	Fn = ~0,

	Reserved, Err_RollOver, Err_PostFail, Err_Undef,

	A, B, C, D, E, F, G, H, I, J, K, L, M,
	N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
	Num_1/*1!*/, Num_2/*2@*/, Num_3/*3#*/, Num_4/*4$*/, Num_5/*5%*/,
	Num_6/*6^*/, Num_7/*7&*/, Num_8/*8**/, Num_9/*9(*/, Num_0/*0)*/,

	Enter, Esc, BSpace, Tab, Space,
	Minus/*-_*/, Equal/*=+*/, Bracket_L/*[{*/, Bracket_R/*]}*/,
	BSlash/*\|*/, Non_US/**/, Semicolon/*;:*/, Quote/*'"*/,
	BQuote/*`~*/, Comma/*,<*/, Period/*.>*/, Slash/*/?*/,

	Lock_Caps,

	F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
	PrtSc,

	Lock_Scroll,

	Pause, Ins, Home, PgUp, Del, End, PgDn,
	Arrow_R, Arrow_L, Arrow_D, Arrow_U,

	Lock_PadNum,

	Pad_Divide, Pad_Multiply, Pad_Subtract, Pad_Add, Pad_Enter,
	Pad_Num_1, Pad_Num_2, Pad_Num_3, Pad_Num_4, Pad_Num_5,
	Pad_Num_6, Pad_Num_7, Pad_Num_8, Pad_Num_9, Pad_Num_0,
	Pad_Dot,

	Non_US_BSlash, Application, Power, Pad_Equal,
	F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24,
	Execute, Help, Menu, Select, Stop,
	Again, Undo, Cut, Copy, Paste, Find,
	Mute, Volume_U, Volume_D
}hid_key_id;	// 暂不用该enum名



#endif // _HHID_DEF_H_
