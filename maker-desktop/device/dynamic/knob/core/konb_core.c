#include "knob_core.h"

#include "ddef.h"
#include "ttool.h"

#include "ssle.h"
#include "hhid.h"



static u8* sle_data;
u8 key_null = 0;


static s8 knob_core_diff(u8 x, u8 y) {
	if (x == 9 && y == 0)
		return 1;

	if (x == 0 && y == 9)
		return -1;

	return y - x;
}


// CH9329
static void knob_core_sle_write_hid_wp(u8 cmd, u8 data_len, const u8* data, u8 data_ctrl) {
	hid_pack_t* hid_wp = (hid_pack_t*)hid_set_wp(cmd, data_len, data, NULL);

	// 假设 cmd == HID_CH9329_CMD_SEND_KB_GENERAL_DATA
	if (data_ctrl) {
		hid_wp->data[0] = data_ctrl;
		hid_wp->data[hid_wp->length] += data_ctrl;
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
			1, &dir, 0
		);

		v_last = value;

		LOG("");
	}

	// LOG("");
}


void knob_core_volume(u8 value) {
	static u8 v_last = 5;

	if (v_last != value) {
		knob_core_diff(v_last, value) > 0
			? sle_write(deskaide, (u8*)"snd volume +", 12)
			: sle_write(deskaide, (u8*)"snd volume -", 12);

		const u32 dir = knob_core_diff(v_last, value) > 0
			? Volume_Up
			: Volume_Down;

		knob_core_sle_write_hid_wp(
			HID_CH9329_CMD_SEND_KB_MEDIA_DATA,
			3, (u8*)&dir, 0
		);

		knob_core_sle_write_hid_wp(
			HID_CH9329_CMD_SEND_KB_MEDIA_DATA,
			3, &key_null, 0
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
			? sle_write(deskaide, (u8*)"snd chapter +", 13)
			: sle_write(deskaide, (u8*)"snd chapter -", 13);

		const u32 dir = knob_core_diff(v_last, value) > 0
			? Prev_Track
			: Next_Track;

		knob_core_sle_write_hid_wp(
			HID_CH9329_CMD_SEND_KB_MEDIA_DATA,
			3, (u8*)&dir, 0
		);

		knob_core_sle_write_hid_wp(
			HID_CH9329_CMD_SEND_KB_MEDIA_DATA,
			3, &key_null, 0
		);

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


void knob_core_tab(u8 value) {
	static u8 v_last = 5;

	if (v_last != value) {
		const u8 ctrl = knob_core_diff(v_last, value) > 0
			? 0
			: 1 << (~Shift_L - 1);

		u8 key = Tab;
		knob_core_sle_write_hid_wp(
			HID_CH9329_CMD_SEND_KB_GENERAL_DATA,
			1, &key, ctrl
		);

		knob_core_sle_write_hid_wp(
			HID_CH9329_CMD_SEND_KB_GENERAL_DATA,
			1, &key_null, 0
		);

		v_last = value;

		LOG("");
	}

	// LOG("");
}
