#include "hhid.h"
#include "hhid_def.h"

#if defined(CONFIG_COMM_FORMAT_HID_XXX)
#	include "xxx/hhid_xxx.h"
#elif defined(CONFIG_COMM_FORMAT_HID_CH9329)
#	include "ch9329/hhid_ch9329.h"
#endif



void hid_init(void) {
#	if defined(CONFIG_COMM_FORMAT_HID_XXX)
	hid_xxx_init();
#	elif defined(CONFIG_COMM_FORMAT_HID_CH9329)
	hid_ch9329_init();
#	endif
}


const hid_pack_t* hid_set_wp(u8 cmd, u8 data_len, const u8* data, const s16* data_map) {
#	if defined(CONFIG_COMM_FORMAT_HID_XXX)
	hid_xxx_set_wp(cmd, data_len, data, data_map);
#	elif defined(CONFIG_COMM_FORMAT_HID_CH9329)
	hid_ch9329_set_wp(cmd, data_len, data, data_map);
#	endif

	return hid_get_wp();
}


const hid_pack_t* hid_get_wp(void) {
#	if defined(CONFIG_COMM_FORMAT_HID_XXX)
	return hid_xxx_get_wp();
#	elif defined(CONFIG_COMM_FORMAT_HID_CH9329)
	return hid_ch9329_get_wp();
#	endif
}
