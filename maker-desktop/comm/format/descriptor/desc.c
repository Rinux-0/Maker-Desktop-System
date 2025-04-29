#include "desc.h"

#include "ddef.h"



bool desc_is_inited = false;



// 全键无冲
const u8 desc_hid_report[] = {
	0x05, 0x01,			//	Usage Page (Generic Desktop),
	0x09, 0x06,			//	Usage (Keyboard),
	0xA1, 0x01,			//	Collection (Application),
	// in-修饰键
	0x05, 0x07,			//		Usage Page (Keyboard),
	0x95, 0x08,			//		Report Count (8),
	0x75, 0x01,			//		Report Size  (1),
	0x15, 0x00,			//		Logical Minimum (0),
	0x25, 0x01,			//		Logical Maximum (1),
	0x19, 0xE0,			//		Usage Minimum (Keyboard LeftControl),
	0x29, 0xE7,			//		Usage Maximum (Keyboard Right GUI),
	0x81, 0x02,			//		Input (Data, Variable, Absolute),
	// in-普通键
	0x05, 0x07,			//		Usage Page (Keyboard),
	0x95, 0x78,			//		Report Count (120),
	0x75, 0x01,			//		Report Size  (1),
	0x15, 0x00,			//		Logical Minimum (0),
	0x25, 0x01,			//		Logical Maximum (1),
	0x19, 0x00,			//		Usage Minimum (0),
	0x29, 0x65,			//		Usage Maximum (101),
	0x81, 0x02,			//		Input (Data, Variable, Absolute),
	// out-LEDs
	0x05, 0x08,			//		Usage Page (LEDs)
	0x95, 0x03,			//		Report Count (3)
	0x75, 0x01,			//		Report Size  (1)
	0x19, 0x01,			//		Usage Minimum (Num Lock 1)
	0x29, 0x03,			//		Usage Maximum (Scroll Lock 3)
	0x91, 0x02,			//		Output (Data,Var,Abs)
	// out-凑bit (无用)
	0x95, 0x05,			//		Report Count (5)
	0x75, 0x01,			//		Report Size  (1)
	0x91, 0x01,			//		Output (Cnst,Var,Abs)

	0xC0				//	End Collection
};
