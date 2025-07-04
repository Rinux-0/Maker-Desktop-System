#pragma once
#ifndef _DYNAMIC_DEF_H_
#define _DYNAMIC_DEF_H_



#if !defined(CONFIG_DEVICE_DYNAMIC_FINGERPRINT)
#	define fingerprint_init()
#	define fingerprint_oneloop()
#	define fingerprint_exit()
#	define fingerprint_entry()
#endif
#if !defined(CONFIG_DEVICE_DYNAMIC_KNOB)
#	define knob_init()
#	define knob_oneloop()
#	define knob_exit()
#	define knob_entry()
#endif
#if !defined(CONFIG_DEVICE_DYNAMIC_NFC)
#	define nfc_init()
#	define nfc_oneloop()
#	define nfc_exit()
#	define nfc_entry()
#endif



#include "ddef.h"

typedef struct {
	u8 id;
	u16 grade;
	bool is_valid;
} fingerprint_t;



#endif // _DYNAMIC_DEF_H_
