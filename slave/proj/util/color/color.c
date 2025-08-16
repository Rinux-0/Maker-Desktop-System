#include "color.h"

#include "ddef.h"
#include "ttool.h"

#include "core/color_core.h"
#include "color_def.h"
#include "mode/mode.h"



void color_show_0(u16 num, double dev_spd) {
	/* DBG */
	// DATA("[V: %d.%d%d] -> [RGB: %d, %d, %d]\n",
	// 	(u16)hsv.v, (u8)(hsv.v * 10) % 10, (u8)(hsv.v * 100) % 10,
	// 	rgb.r, rgb.g, rgb.b
	// );

	switch (color_get_mode()) {
	default:	// 复原 全局参数
		hsv.s = 1.f;
		color_h_speed = 1;
		color_v_max = .5f;
		color_v_is_changing = false;
		color_set_mode(COLOR_MODE_OFF);
	/****/case COLOR_MODE_OFF:		color_core_reset(0, num);
	break;case COLOR_MODE_BREATH:	color_show_mode_breath(num, dev_spd);
	break;case COLOR_MODE_FLOW:		color_show_mode_flow(num, dev_spd);
	break;case COLOR_MODE_MESS:		color_show_mode_mess(num, dev_spd);
	break;case COLOR_MODE_STATIC:	color_show_mode_static(num, dev_spd);	// 放在最后，便于恢复全局参数
	}
}


void color_show_1(u16 num, u8* spi_buff) {
	color_core_show(1, num, spi_buff);
}


void color_init(void) {}


void color_oneloop(void) {}


void color_exit(void) {}
