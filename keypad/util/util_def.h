#ifndef _UTIL_DEF_H_
#define _UTIL_DEF_H_



#include <stdbool.h>	// bool相关
#include <stddef.h>		// NULL
#include <stdio.h>		// sprintf()
// #include <string.h>		// str处理相关
// #include <stdint.h>		// uint8_t, uint16_t, uint32_t, uint64_t

// keypad配置
#define HC165_CE_PIN	2
#define HC165_CP_PIN	7	// CLK-手动
#define HC165_PL_PIN	10
#define HC165_Q7_PIN 	11	// MISO
#define NUM_KEY			16
#if NUM_KEY % 8
#	define NUM_REGISTER NUM_KEY / 8 + 1
#else
#	define NUM_REGISTER NUM_KEY / 8
#endif

// util配置
#define LED_PIN 	13	// 红LED

// 其他
typedef unsigned char		u8;		// uint8_t
typedef unsigned short		u16;	// uint16_t
typedef unsigned int		u32;	// uint32_t
typedef unsigned long long	u64;	// uint64_t



#endif // _UTIL_DEF_H_
