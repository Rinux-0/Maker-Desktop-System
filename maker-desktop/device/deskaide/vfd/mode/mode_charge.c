#include "mode.h"

#include "ddef.h"
#include "ttool.h"

#include "../vfd_def.h"
#include "../core/vfd_core.h"

#include "usbhub.h"



static u8 block_pos = 0;
static u8 ch_load = '\334';



void vfd_show_mode_charge(void) {
	u8 vfd_l[2][21];

	if (block_pos++ >= 20)
		block_pos = 0;

	switch ((c8)ch_load) {
	default:			ERROR("");
	break;case '\335':	ch_load = '\334';
	break;case '\334':	ch_load = '\336';
	break;case '\336':	ch_load = '\337';
	break;case '\337':	ch_load = '\335';
	}

	strcpy((c8*)&vfd_l[0], "....................");
	vfd_l[0][block_pos] = '\333';

	// 获取电压、电流、功率
	usb_output_t* output = (usb_output_t*)usbhub_get_output();

	sprintf((c8*)(&vfd_l[1]), "%c%c>%2d.%1dA  %s... ",
		output->current_a > .15f ? '>' : ch_load,
		output->current_a > .15f ? '>' : ch_load,
		(u8)output->current_a, (u8)(output->current_a * 10) % 10,
		output->current_a > .15f ? "chging" : "idling"
	);

	vfd_core_set_screen(vfd_l[0], vfd_l[1], true);
	vfd_core_set_pos(1, 1);
}
