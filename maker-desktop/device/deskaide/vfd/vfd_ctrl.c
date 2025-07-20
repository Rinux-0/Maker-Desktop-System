#include "vfd.h"

#include "ddef.h"
#include "ttool.h"

#include "vfd_def.h"



static vfd_mode_t vfd_mode = VFD_MODE_OFF;



vfd_mode_t vfd_get_mode(void) {
	return vfd_mode;
}


void vfd_set_mode(vfd_mode_t mode) {
	vfd_mode = mode;

	DATA("[vfd mode: %d]\n", vfd_mode);
}


void vfd_set_mode_next(void) {
	vfd_mode = vfd_mode + 1;	// 不做求余，方便每周期复原一次全局参数

	DATA("[vfd mode: %d]\n", vfd_mode);
}
