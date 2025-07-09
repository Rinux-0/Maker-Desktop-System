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
	static u8 v_last = 5;

	if (v_last != value) {
		const u8 dir = knob_core_diff(v_last, value) > 0 ? 1 : 0;

		knob_core_sle_write_hid_wp(
			HID_CH9329_CMD_SEND_MS_REL_DATA,
			dir, 0
		);

		v_last = value;

		LOG("");
	}

	// LOG("");
}


void knob_core_volume(u8 value) {
	static u8 v_last = 5;

	if (v_last != value) {
		const u8 dir = knob_core_diff(v_last, value) > 0
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

		v_last = value;

		LOG("");
	}

	// LOG("");
}


void knob_core_music(u8 value) {
	static u8 v_last = 5;

	if (v_last != value) {
		knob_core_diff(v_last, value) > 0
			? sle_write(deskaide, (u8*)"snd odr +", 9)
			: sle_write(deskaide, (u8*)"snd odr -", 9);

		v_last = value;

		LOG("");
	}

	// LOG("");
}


void knob_core_lamp_light(u8 value) {
	static u8 v_last = 5;

	if (v_last != value) {
		if (knob_core_diff(v_last, value) > 0) {
			sle_data = (u8*)"lamp light +";
		} else {
			sle_data = (u8*)"lamp light -";
		}

		sle_write(health, sle_data, strlen((char*)sle_data));

		v_last = value;

		LOG("");
	}

	// LOG("");
}


void knob_core_lamp_hue(u8 value) {
	static u8 v_last = 5;

	if (v_last != value) {
		if (knob_core_diff(v_last, value) > 0) {
			sle_data = (u8*)"lamp hue +";
		} else {
			sle_data = (u8*)"lamp hue -";
		}

		sle_write(health, sle_data, strlen((char*)sle_data));

		v_last = value;

		LOG("");
	}

	// LOG("");
}


// void knob_core_change_window(u8 value) {
// 	static u8 v_last = 5;

// 	if (v_last != value) {
// 		const u8 ctrl = knob_core_diff(v_last, value) > 0
// 			? 1 << (~Alt_L - 1)
// 			: (1 << (~Alt_L - 1)) | (1 << (~Shift_L - 1));

// 		knob_core_sle_write_hid_wp(
// 			HID_CH9329_CMD_SEND_KB_GENERAL_DATA,
// 			Tab, ctrl
// 		);	// 切换窗口

// 		knob_core_sle_write_hid_wp(
// 			HID_CH9329_CMD_SEND_KB_GENERAL_DATA,
// 			0, 0
// 		);

// 		v_last = value;

// 		LOG("");
// 	}

// 	// LOG("");
// }


void knob_core_tab(u8 value) {
	static u8 v_last = 5;

	if (v_last != value) {
		const u8 ctrl = knob_core_diff(v_last, value) > 0
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

		v_last = value;

		LOG("");
	}

	// LOG("");
}
