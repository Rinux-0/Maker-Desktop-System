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


void knob_core_mouse_scroll(u8 value) {
	static u8 v = 0;

	if (v != value) {
		/// @todo sle_data 更新

		//hid_set_wp(, value);

		// sle_write(0, sle_data, 0);

		LOG("\n\t%d\n\n", knob_core_diff(v, value));

		v = value;
	}

	// LOG("");
}


void knob_core_volume(u8 value) {
	static u8 v = 0;

	if (v != value) {
		/// @todo sle_data 更新
		// sle_write(0, sle_data, 0);

		LOG("\n\t%d\n\n", knob_core_diff(v, value));

		v = value;
	}

	// LOG("");
}


void knob_core_music(u8 value) {
	static u8 v = 0;

	// if (v != value) {
	// 	/// @todo sle_data 更新
	// 	sle_write(0, sle_data, 0);

	// 	LOG("\n\t%d\n\n", knob_core_diff(v, value));

	// 	v = value;
	// }

	// LOG("");
}


void knob_core_lamp_light(u8 value) {
	static u8 v = 0;

	// if (v != value) {
	// 	/// @todo sle_data 更新
	// 	sle_write(0, sle_data, 0);

	// 	LOG("\n\t%d\n\n", knob_core_diff(v, value));

	// 	v = value;
	// }

	// LOG("");
}


void knob_core_screen_light(u8 value) {
	static u8 v = 0;

	if (v != value) {
		/// @todo sle_data 更新
		// sle_write(0, sle_data, 0);

		LOG("\n\t%d\n\n", knob_core_diff(v, value));

		v = value;
	}

	// LOG("");
}


void knob_core_tab(u8 value) {
	static u8 v = 0;

	if (v != value) {
		/// @todo sle_data 更新
		// sle_write(0, sle_data, 0);

		LOG("\n\t%d\n\n", knob_core_diff(v, value));

		v = value;
	}

	// LOG("");
}
