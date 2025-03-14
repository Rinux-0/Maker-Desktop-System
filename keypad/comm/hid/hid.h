#ifndef _HID_H_
#define _HID_H_



#include "../../util/util_def.h"
#include "hid_cmd_def.h"

#define PACK_DATA_SIZE_MAX	64

/// @attention 以下
/// @param data_sum
//		为方便发送包，（暂定）最后还需执行：
//			包.data_sum[length] = 包.sum
/// @param sum = 无data_sum时的值 + data_sum
// 		（手册的计算法则有误，需参考不同cmd具体包的例子）
#pragma pack(1)
typedef struct {
	u8 head[2];
	u8 addr;
	u8 cmd;
	u8 length;		// length <= PACK_DATA_SIZE_MAX
	u8 data_sum[PACK_DATA_SIZE_MAX];
	u8 sum;			// 包校验和
}HID_Pack;

HID_Pack hid_write_pack_construct(u8 cmd);
HID_Pack hid_read_pack_destruct(u8 cmd);
const HID_Pack* get_hid_w_pack(void);



#endif // _HID_H_
