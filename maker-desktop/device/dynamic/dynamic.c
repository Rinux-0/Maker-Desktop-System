#include "dynamic.h"
#include "dynamic_def.h"

#include "ddef.h"
#include "ttool.h"

#if defined(CONFIG_DEVICE_DYNAMIC_FINGERPRINT)
#	include "fingerprint/fingerprint.h"
#endif
#if defined(CONFIG_DEVICE_DYNAMIC_KNOB)
#	include "knob/knob.h"
#endif
#if defined(CONFIG_DEVICE_DYNAMIC_NFC)
#	include "nfc/nfc.h"
#endif



void dynamic_init(void) {
	/*1*/fingerprint_init();
	/*2*/knob_init();
	/*3*/nfc_init();
}


void dynamic_oneloop(void) {
	/*1*/fingerprint_oneloop();
	/*2*/knob_oneloop();
	/*3*/nfc_oneloop();
}


void dynamic_exit(void) {
	/*1*/fingerprint_exit();
	/*2*/knob_exit();
	/*3*/nfc_exit();
}
