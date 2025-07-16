#include "deskaide.h"

#include "ddef.h"
#include "ttool.h"

#include "deskaide_def.h"
#include "ssle.h"
#if defined(CONFIG_DEVICE_DESKAIDE_ASRPRO)
#	include "asrpro.h"
#endif
#if defined(CONFIG_DEVICE_DESKAIDE_SOUND)
#	include "sound.h"
#endif
#if defined(CONFIG_DEVICE_DESKAIDE_VFD)
#	include "vfd.h"
#endif



void deskaide_sle_r_int_handler(u8 cs_id, u16 conn_id, ssle_ssap_value_t* read_cb_para, errcode_t status) {
	sound_sle_r_int_handler(cs_id, conn_id, read_cb_para, status);
	vfd_sle_r_int_handler(cs_id, conn_id, read_cb_para, status);
}


void deskaide_init(void) {
	sle_set_r_cb(deskaide_sle_r_int_handler);

	// asrpro_init();
	// sound_init();
	// vfd_init();

	asrpro_entry();
	sound_entry();
	vfd_entry();
}


void deskaide_oneloop(void) {
	tool_sleep_m(1);

	// asrpro_oneloop();
	// sound_oneloop();
	// vfd_oneloop();
}


void deskaide_exit(void) {
	// asrpro_exit();
	// sound_exit();
	// vfd_exit();
}
