#include "knob_core.h"

#include "ddef.h"
#include "ttool.h"

#include "ssle.h"
#include "hhid.h"



u8* sle_data;



static s8 knob_core_diff(u8 x, u8 y) {
	if (x == 9 && y == 0)
		return 1;

	if (x == 0 && y == 9)
		return -1;

	return y - x;
}


// CH9329
static void knob_core_sle_write_hid_wp(u8 cmd, u8 data, u8 data_ctrl) {
	hid_pack_t* hid_wp = (hid_pack_t*)hid_set_wp(cmd, 1, &data, NULL);

	if (data_ctrl) {
		hid_wp->data[0] = data_ctrl;
		hid_wp->data[8] += data_ctrl;
		// hid_wp->sum += data_ctrl;
	}

	sle_write(pc, (u8*)hid_wp, hid_wp->length + 6);
}


void knob_core_mouse_scroll(u8 value) {
	static u8 v = 5;

	if (v != value) {
		const u8 dir = knob_core_diff(v, value) > 0 ? 1 : 0;

		knob_core_sle_write_hid_wp(
			HID_CH9329_CMD_SEND_MS_REL_DATA,
			dir, 0
		);

		v = value;

		LOG("");
	}

	// LOG("");
}


void knob_core_volume(u8 value) {
	static u8 v = 5;

	if (v != value) {
		const u8 dir = knob_core_diff(v, value) > 0
			? Volume_U
			: Volume_D;

		knob_core_sle_write_hid_wp(
			HID_CH9329_CMD_SEND_KB_GENERAL_DATA,
			dir, 0
		);

		knob_core_sle_write_hid_wp(
			HID_CH9329_CMD_SEND_KB_GENERAL_DATA,
			0, 0
		);

		v = value;

		LOG("");
	}

	// LOG("");
}


void knob_core_music(u8 value) {
	static u8 v = 5;

	if (v != value) {
		knob_core_diff(v, value) > 0
			? sle_write(trinity, (u8*)"snd + odr", 9)
			: sle_write(trinity, (u8*)"snd - odr", 9);

		v = value;

		LOG("");
	}

	// LOG("");
}


void knob_core_lamp_light(u8 value) {
	static u8 v = 5;

	// if (v != value) {
	// 	/// @todo sle_data 更新
	// 	sle_write(pc, sle_data, 0);

	// 	LOG("\n\t%d\n\n", knob_core_diff(v, value));

	// 	v = value;
	// }

	// LOG("");
}


void knob_core_change_window(u8 value) {
	static u8 v = 5;

	if (v != value) {
		const u8 ctrl = knob_core_diff(v, value) > 0
			? 1 << (~Alt_L - 1)
			: (1 << (~Alt_L - 1)) | (1 << (~Shift_L - 1));

		knob_core_sle_write_hid_wp(
			HID_CH9329_CMD_SEND_KB_GENERAL_DATA,
			Tab, ctrl
		);	// 切换窗口

		knob_core_sle_write_hid_wp(
			HID_CH9329_CMD_SEND_KB_GENERAL_DATA,
			0, 0
		);

		v = value;

		LOG("");
	}

	// LOG("");
}


void knob_core_tab(u8 value) {
	static u8 v = 5;

	if (v != value) {
		const u8 ctrl = knob_core_diff(v, value) > 0
			? 0
			: 1 << (~Shift_L - 1);

		knob_core_sle_write_hid_wp(
			HID_CH9329_CMD_SEND_KB_GENERAL_DATA,
			Tab, ctrl
		);

		knob_core_sle_write_hid_wp(
			HID_CH9329_CMD_SEND_KB_GENERAL_DATA,
			0, 0
		);

		v = value;

		LOG("");
	}

	// LOG("");
}
