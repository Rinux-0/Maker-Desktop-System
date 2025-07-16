#pragma once
#ifndef _KEYTEST_DEF_H_
#define _KEYTEST_DEF_H_



#include "ddef.h"

#define KTT_PIN_74HC165_CE	(2)
#define KTT_PIN_74HC165_CP	(7)		// CLK-手动
#define KTT_PIN_74HC165_PL	(10)
#define KTT_PIN_74HC165_Q7 	(11)	// MISO

#define KTT_NUM_KEY			NUM_KEY
#if KTT_NUM_KEY % 8
#	define KTT_NUM_REGISTER (KTT_NUM_KEY / 8 + 1)
#else
#	define KTT_NUM_REGISTER (KTT_NUM_KEY / 8)
#endif



#endif // _KEYTEST_DEF_H_
