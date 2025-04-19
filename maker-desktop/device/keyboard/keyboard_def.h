#pragma once
#ifndef _KEYBOARD_DEF_H_
#define _KEYBOARD_DEF_H_



#define KBD_74HC165_PIN_CE	(2)
#define KBD_74HC165_PIN_CP	(9)		// CLK-手动
#define KBD_74HC165_PIN_PL	(10)
#define KBD_74HC165_PIN_Q7 	(11)	// MISO

#define KBD_NUM_KEY			(84)
#if KBD_NUM_KEY % 8
#	define KBD_NUM_REGISTER (KBD_NUM_KEY / 8 + 1)
#else
#	define KBD_NUM_REGISTER (KBD_NUM_KEY / 8)
#endif



#endif // _KEYBOARD_DEF_H_
