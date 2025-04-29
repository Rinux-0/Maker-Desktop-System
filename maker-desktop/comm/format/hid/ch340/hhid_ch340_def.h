#pragma once
#ifndef _HHID_CH340_DEF_H_
#define _HHID_CH340_DEF_H_



#include "ddef.h"

#pragma pack(1)
typedef struct {
	u8  data[64];	/// @todo
	u8  length;
}hid_ch340_pack;



#endif // _HHID_CH340_DEF_H_
