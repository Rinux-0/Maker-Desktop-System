#include "trinity.h"

#include "ddef.h"
#include "ttool.h"

#include "trinity_def.h"
#if defined(CONFIG_DEVICE_TRINITY_ASRPRO)
#	include "asrpro.h"
#endif
#if defined(CONFIG_DEVICE_TRINITY_SOUND)
#	include "sound.h"
#endif
#if defined(CONFIG_DEVICE_TRINITY_VFD)
#	include "vfd.h"
#endif



void trinity_init(void) {
	// asrpro_init();
	// sound_init();
	// vfd_init();

	sound_entry();
	vfd_entry();
}


void trinity_oneloop(void) {
	tool_delay_m(1);

	// asrpro_oneloop();
	// sound_oneloop();
	// vfd_oneloop();
}


void trinity_exit(void) {
	// asrpro_exit();
	// sound_exit();
	// vfd_exit();
}
