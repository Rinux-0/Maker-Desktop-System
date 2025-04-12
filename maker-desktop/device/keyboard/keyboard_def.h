#pragma once
#ifndef _KEYBOARD_DEF_H_
#define _KEYBOARD_DEF_H_



#define HC165_CE_PIN	(2)
#define HC165_CP_PIN	(9)	// CLK-手动
#define HC165_PL_PIN	(10)
#define HC165_Q7_PIN 	(11)	// MISO

#define NUM_KEY			(84)
#if NUM_KEY % 8
#	define NUM_REGISTER (NUM_KEY / 8 + 1)
#else
#	define NUM_REGISTER (NUM_KEY / 8)
#endif



#endif // _KEYBOARD_DEF_H_
