#pragma once
#ifndef _DYNAMIC_DEF_H_
#define _DYNAMIC_DEF_H_



#if !defined(CONFIG_DEVICE_DYNAMIC_FINGERPRINT)
#	define fingerprint_init()
#	define fingerprint_oneloop()
#	define fingerprint_exit()
#endif
#if !defined(CONFIG_DEVICE_DYNAMIC_KNOB)
#	define knob_uart_r_int_handler(buffer, length, error)
#	define knob_init()
#	define knob_oneloop()
#	define knob_exit()
#endif
#if !defined(CONFIG_DEVICE_DYNAMIC_NFC)
#	define nfc_uart_r_int_handler(buffer, length, error)
#	define nfc_init()
#	define nfc_oneloop()
#	define nfc_exit()
#endif



#include "ddef.h"

typedef struct {
	u8 id;
	u16 grade;
	bool is_valid;
} fingerprint_t;

// typedef struct {
// 	u8 id;
// } nfc_t;



#endif // _DYNAMIC_DEF_H_
