#include "color.h"

#include "ddef.h"
#include "ttool.h"

#include "color_def.h"
#include "core/color_core.h"
#include "mode/mode.h"



color_mode_t color_get_mode(void) {
	return color_mode;
}


void color_set_mode(color_mode_t mode) {
	if ((color_mode == COLOR_MODE_FLOW) && (color_v_max > 1.f))
		color_v_max = 1.f;

	color_mode = mode;

	DATA("[COLOR MODE: %d]\n", color_mode);
}


void color_set_mode_next(void) {
	color_mode = color_mode + 1;	// 不做求余，方便每周期复原一次全局参数

	DATA("[COLOR MODE: %d]\n", color_mode);
}


void color_ctrl_hsv_h(s8 speed) {
	if (speed == 0) {					// 周期方向递变
		static s8 dir = 1;
		color_h_speed += dir;
		if (abs(color_h_speed) == 5)
			dir = -dir;
	} else if (abs(speed) <= 5) {
		color_h_speed = speed;
	} else {							// 控制方向递变
		s8 dir = (speed > 0) * 2 - 1;
		color_h_speed += dir;
	}

	if (color_h_speed > 5) {
		color_h_speed = (color_mode == COLOR_MODE_STATIC) ? -5 : 5;
	} else if (color_h_speed < -5) {
		color_h_speed = (color_mode == COLOR_MODE_STATIC) ? 5 : -5;
	}

	DATA("\n\n\tspeed:%d\n\n\n", color_h_speed);
}


void color_ctrl_hsv_v(bool is_changing, float v_max, u8 which_is_valid) {
	if (which_is_valid & 0b10) {
		color_v_is_changing = is_changing;
	} else if (is_changing) {
		color_v_is_changing = !color_v_is_changing;
	}

	if (which_is_valid & 0b01) {
		color_v_max = v_max;
	} else if (v_max > +.1f) {
		color_v_max += .1f;
	} else if (v_max < -.1f) {
		color_v_max -= .1f;
	}

	if (color_mode == COLOR_MODE_FLOW) {
		if (color_v_max > 1.1f) {
			color_v_max = 1.1f;
		} else if (color_v_max < 0.f) {
			color_v_max = 0.f;
		}
	} else if (color_v_max > 1.f) {
		color_v_max = 1.f;
	} else if (color_v_max < 0.f) {
		color_v_max = 0.f;
	}

	DATA("\n\tcolor_v_max:%.2f\n\n", color_v_max);
}


void color_ctrl_hsv(color_hsv_t color_hsv) {
	hsv = color_hsv;
}
