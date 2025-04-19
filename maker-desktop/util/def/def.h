#pragma once
#ifndef _UTIL_DEF_H_
#define _UTIL_DEF_H_



#if !defined(CONFIG_DEBUG) && !defined(CONFIG_RELEASE)
#	define CONFIG_DEBUG		// 方便编码（*）
#	define CONFIG_RELEASE	// 方便编码
#endif



#include <stdbool.h>
#include <common_def.h>

// 必需两层宏定义，以保证宏参被展开
#define STR_X(x) #x
#define STR(x) STR_X(x)
#define CONCAT2_X(x, y) x ## y
#define CONCAT2(x, y) CONCAT2_X(x, y)
#define CONCAT3_X(x, y, z) x ## y ## z
#define CONCAT3(x, y, z) CONCAT3_X(x, y, z)

typedef signed char			int8_t;
typedef signed short		int16_t;
typedef signed int			int32_t;
typedef signed long long	int64_t;

typedef unsigned char		uint8_t;
typedef unsigned short		uint16_t;
typedef unsigned int		uint32_t;
typedef unsigned long long	uint64_t;

typedef int8_t  s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

#if defined(CONFIG_DEVICE_KEYBOARD)
#	define LED_PIN 12
#elif defined(CONFIG_DEVICE_KEYPAD)
#	define LED_PIN 13
#else
#	define LED_PIN PIN_NONE
// #error "暂未开发其他设备的LED控制"
#endif



#endif // _UTIL_DEF_H_
