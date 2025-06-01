#pragma once
#ifndef _HHID_DEF_H_
#define _HHID_DEF_H_



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

/// @todo 可统一
#if defined(CONFIG_COMM_FORMAT_HID_CH340)
#	include "ch340/hhid_ch340_def.h"
typedef hid_ch340_pack_t hid_pack_t;
#elif defined(CONFIG_COMM_FORMAT_HID_CH9329)
#	include "ch9329/hhid_ch9329_def.h"
typedef hid_ch9329_pack_t hid_pack_t;
#endif



#endif // _HHID_DEF_H_
