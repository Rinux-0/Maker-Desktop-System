#include "color.h"

#include "ddef.h"
#include "ttool.h"

#include "core/color_core.h"
#include "color_def.h"
#include "mode/mode.h"



static color_mode_t color_mode = COLOR_MODE_OFF;



color_mode_t color_get_mode(void) {
	return color_mode;
}


void color_set_mode(color_mode_t mode) {
	color_mode = mode;

	DATA("[COLOR MODE: %d]\n", color_mode);
}


void color_set_mode_next(void) {
	color_mode = color_mode + 1;	// 不做求余，方便每周期复原一次全局参数

	DATA("[COLOR MODE: %d]\n", color_mode);
}


void color_show(u8 spi_bus_id, u16 num, double dev_spd) {
	/* DBG */
	// DATA("[V: %d.%d%d] -> [RGB: %d, %d, %d]\n",
	// 	(u16)hsv.v, (u8)(hsv.v * 10) % 10, (u8)(hsv.v * 100) % 10,
	// 	rgb.r, rgb.g, rgb.b
	// );

	if (spi_bus_id == 0) {
		switch (color_mode) {
		default:	// 复原 全局参数
			color_h_speed = 1;
			color_s_is_full = true;
			color_v_is_full = true;
			color_s_is_changing = false;
			color_v_is_changing = false;
			color_mode = COLOR_MODE_OFF;
		/****/case	COLOR_MODE_OFF:			color_core_reset(0, num);
		break;case	COLOR_MODE_BREATH:		color_show_mode_breath(num, dev_spd);
		break;case	COLOR_MODE_FLOW:		color_show_mode_flow(num, dev_spd);
		break;case	COLOR_MODE_MESS:		color_show_mode_mess(num, dev_spd);
		break;case	COLOR_MODE_STATIC:		color_show_mode_static(num, dev_spd);	// 放在最后，便于恢复全局参数
		}
	} else {
		color_core_show(1);
	}
}


void color_init(void) {}


void color_oneloop(void) {}


void color_exit(void) {}
