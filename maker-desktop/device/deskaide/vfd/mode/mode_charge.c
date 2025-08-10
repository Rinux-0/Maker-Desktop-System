#include "mode.h"

#include "ddef.h"
#include "ttool.h"

#include "../vfd_def.h"
#include "../core/vfd_core.h"

#include "usbhub.h"



static u8 block_pos = 0;
static u8 ch_load[2] = { '\334', '\334' };



void vfd_show_mode_charge(void) {
	u8 vfd_l[2][21];

	if (block_pos++ >= 20)
		block_pos = 0;

	switch ((c8)ch_load[0]) {
	default:			ERROR("");
	break;case '\335':	ch_load[0] = '\334';
	break;case '\334':	ch_load[0] = ' ';
	break;case ' ':		ch_load[0] = '\337';
	break;case '\337':	ch_load[0] = '\335';
	}
	switch ((c8)ch_load[1]) {
	default:			ERROR("");
	break;case ' ':		ch_load[1] = '\334';
	break;case '\334':	ch_load[1] = '\336';
	break;case '\336':	ch_load[1] = '\337';
	break;case '\337':	ch_load[1] = ' ';
	}

	strcpy((c8*)&vfd_l[0], "....................");
	vfd_l[0][block_pos] = '\333';

	// 获取电压、电流、功率
	usb_output_t* output = (usb_output_t*)usbhub_get_output();

	// for (u8 i = 0; i < 2; i++)
	// 	sprintf((c8*)(&vfd_l[i]), "%c%c>%2d.%1dA  %s... ",
	// 		output[i].current_a > .15f ? '>' : ch_load,
	// 		output[i].current_a > .15f ? '>' : ch_load,
	// 		(u8)output[i].current_a, (u8)(output[i].current_a * 10) % 10,
	// 		output[i].current_a > .15f ? "chging" : "idling"
	// 	);

	sprintf((c8*)(&vfd_l[1]), "%c%c>%2d.%1dA \333\333 %c%c>%2d.%1dA",
		output[0].current_a > .15f ? '>' : ch_load[0],
		output[1].current_a > .15f ? '>' : ch_load[1],
		(u8)output[0].current_a, (u8)(output[0].current_a * 10) % 10,
		output[0].current_a > .15f ? '>' : ch_load[0],
		output[1].current_a > .15f ? '>' : ch_load[1],
		(u8)output[1].current_a, (u8)(output[1].current_a * 10) % 10
	);

	vfd_core_set_screen(vfd_l[0], vfd_l[1], true);
}
