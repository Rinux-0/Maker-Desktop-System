#include "core/color_core.h"

#include "ddef.h"
#include "ttool.h"

#include "color_def.h"



void color_ctrl_hsv_h(s8 speed) {
	if (speed == 0) {						// 周期方向递变
		static s8 dir = 1;
		color_h_speed += dir;
		if (abs(color_h_speed) == 3)
			dir = -dir;
	} else if (abs(speed) <= 3) {
		color_h_speed = speed;
	} else {	// 控制方向递变
		s8 dir = (speed > 0) * 2 - 1;
		color_h_speed += dir;
	}

	if (color_h_speed > 3) {
		color_h_speed = 3;
	} else if (color_h_speed < -3) {
		color_h_speed = -3;
	}

	DATA("\n\n\tspeed:%d\n\n\n", color_h_speed);
}


void color_ctrl_hsv_s(bool is_changing, bool is_full, u8 which_is_valid) {
	if (which_is_valid & 0b00000010) {
		color_s_is_changing = is_changing;
	} else if (is_changing) {
		color_s_is_changing = !color_s_is_changing;
	}

	if (which_is_valid & 0b00000001) {
		color_s_is_full = is_full;
	} else if (is_full) {
		color_s_is_full = !color_s_is_full;
	}

	DATA("change:%d, full:%d\n", color_s_is_changing, color_s_is_full);
}


void color_ctrl_hsv_v(bool is_changing, bool is_full, u8 which_is_valid) {
	if (which_is_valid & 0b00000010) {
		color_v_is_changing = is_changing;
	} else if (is_changing) {
		color_v_is_changing = !color_v_is_changing;
	}

	if (which_is_valid & 0b00000001) {
		color_v_is_full = is_full;
	} else if (is_full) {
		color_v_is_full = !color_v_is_full;
	}
}


void color_ctrl_hsv(u8 spi_bus_id, color_hsv_t color_hsv) {
	hsv[spi_bus_id] = color_hsv;
}
