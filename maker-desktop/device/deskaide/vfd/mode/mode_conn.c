#include "mode.h"

#include "ddef.h"
#include "ttool.h"

#include "../vfd_def.h"
#include "../core/vfd_core.h"

#include "ssle.h"



static u8 vfd_sle_conn_status[4] = { '_', '_', '_', '_' };



void vfd_update_sle_conn_status(u8 index, bool status) {
	vfd_sle_conn_status[index] = status ? '\333' : '_';
}


void vfd_show_mode_conn(void) {
	u8* vfd_l1 = (u8*)"Maker Desktop System";
	u8 vfd_l2[21];
	if (sle_is_connected()) {
		u8 target_id = deskaide;
		sle_write(receiver, &target_id, 1);		// 请求 sle_conn_status
		sprintf((c8*)vfd_l2, "dnm%c hlth%c kbd%c kpd%c",
			vfd_sle_conn_status[0], vfd_sle_conn_status[1],
			vfd_sle_conn_status[2], vfd_sle_conn_status[3]
		);
	} else {
		strcpy((c8*)vfd_l2, "   disconnected...  ");
	}

	vfd_core_set_screen(vfd_l1, vfd_l2, true);
	vfd_core_set_pos(1, 1);
}
