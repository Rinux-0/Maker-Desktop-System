#include "deskaide.h"

#include "ddef.h"
#include "ttool.h"

#include "deskaide_def.h"
#if defined(CONFIG_DEVICE_DESKAIDE_ASRPRO)
#	include "asrpro.h"
#endif
#if defined(CONFIG_DEVICE_DESKAIDE_SOUND)
#	include "sound.h"
#endif
#if defined(CONFIG_DEVICE_DESKAIDE_VFD)
#	include "vfd.h"
#endif



void deskaide_init(void) {
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
