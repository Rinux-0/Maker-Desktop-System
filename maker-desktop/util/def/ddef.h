#pragma once
#ifndef _UTIL_DDEF_H_
#define _UTIL_DDEF_H_



#if !defined(OUTPUT_DEBUG) && !defined(OUTPUT_RELEASE)
#	define OUTPUT_DEBUG		// 方便编码（*）
#	define OUTPUT_RELEASE	// 方便编码
#endif



#define STR_X(x) #x
#define STR(x) STR_X(x)

#define CONCAT2_X(x, y)		x ## y
#define CONCAT2(x, y) 		CONCAT2_X(x, y)
#define CONCAT3_X(x, y, z)	x ## y ## z
#define CONCAT3(x, y, z)	CONCAT3_X(x, y, z)

#define __FILE_R__ (strstr(__FILE__, "maker-desktop") + sizeof("maker-desktop"))	// 相对路径
#define __F_NAME__ (strrchr(__FILE__, '/') + 1)

#define MY_HEADER_NAME(name_main, name_expand)			STR(name_main.name_expand)
#define MY_HEADER_PATH(path, name_main, name_expand)	STR(path/name_main.name_expand)



#include <common_def.h>
#include <errcode.h>
#include <stdbool.h>

typedef char su8;

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



#if defined(CONFIG_TEST_TMP)
#	define LED_PIN_RUN 13
#endif

#if defined(CONFIG_DEVICE_KEYBOARD)
/// @todo UART_TX_1 和 UART_RX_2 可并用到 UART_ID_1
#	define LED_PIN_RUN 12
#	define LED_PIN_SLE 8			// UART_TX_2
#elif defined(CONFIG_DEVICE_UNION) || defined(CONFIG_DEVICE_HEALTH) || defined(CONFIG_DEVICE_DYNAMIC)
#	define LED_PIN_RUN 2
#elif defined(CONFIG_TEST_KEYTEST)
/// @todo UART_TX_1 和 UART_RX_2 可并用到 UART_ID_1
// #	define LED_PIN_RUN 13
#	define LED_PIN_SLE 13
#endif

#if defined(CONFIG_TEST_KEYTEST)
#	define LED_PIN_RUN PIN_NONE
#elif !defined(LED_PIN_RUN)
#	error "未定义 LED_PIN_RUN !"
#endif
#if !defined(LED_PIN_SLE)
#	define LED_PIN_SLE PIN_NONE
#endif

#define TIMERS_NUM 4



#endif // _UTIL_DDEF_H_
