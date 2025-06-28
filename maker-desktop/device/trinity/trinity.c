#include "trinity.h"

#include "ddef.h"
#include "ttool.h"

#include "trinity_def.h"
#if defined(CONFIG_DEVICE_TRINITY_SOUND)
#	include "sound.h"
#endif
#if defined(CONFIG_DEVICE_TRINITY_VFD)
#	include "vfd.h"
#endif



void trinity_init(void) {
	sound_init();
	vfd_init();
}


void trinity_oneloop(void) {
	sound_oneloop();
	vfd_oneloop();
}


void trinity_exit(void) {
	sound_exit();
	vfd_exit();
}
