#pragma once
#ifndef _KEYPAD_DEF_H_
#define _KEYPAD_DEF_H_



#define KPD_74HC165_PIN_CE	(2)
#define KPD_74HC165_PIN_CP	(7)		// CLK-手动
#define KPD_74HC165_PIN_PL	(10)
#define KPD_74HC165_PIN_Q7 	(11)	// MISO

#define KPD_NUM_KEY			(16)
#if KPD_NUM_KEY % 8
#	define KPD_NUM_REGISTER (KPD_NUM_KEY / 8 + 1)
#else
#	define KPD_NUM_REGISTER (KPD_NUM_KEY / 8)
#endif



#endif // _KEYPAD_DEF_H_
