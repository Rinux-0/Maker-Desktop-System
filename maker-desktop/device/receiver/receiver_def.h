#pragma once
#ifndef _KEYPAD_DEF_H_
#define _KEYPAD_DEF_H_



#define RCV_74HC165_PIN_CE	(2)
#define RCV_74HC165_PIN_CP	(7)	// CLK-手动
#define RCV_74HC165_PIN_PL	(10)
#define RCV_74HC165_PIN_Q7 	(11)	// MISO

#define RCV_NUM_KEY			(16)
#if RCV_NUM_KEY % 8
#	define RCV_NUM_REGISTER (RCV_NUM_KEY / 8 + 1)
#else
#	define RCV_NUM_REGISTER (RCV_NUM_KEY / 8)
#endif



#endif // _KEYPAD_DEF_H_
