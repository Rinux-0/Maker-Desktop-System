#pragma once
#ifndef _MY_DEF_H_
#define _MY_DEF_H_



#define __FILE_R__ (strstr(__FILE__, "maker-desktop") + sizeof("maker-desktop"))	// 相对路径
#define __F_NAME__ (strrchr(__FILE__, '/') + 1)



#include <common_def.h>
#include <errcode.h>
#include <stdbool.h>

typedef char c8;

typedef signed char			int8_t;
typedef signed short		int16_t;
typedef signed int			int32_t;
typedef signed long long	int64_t;

typedef unsigned char		uint8_t;
typedef unsigned short		uint16_t;
typedef unsigned int		uint32_t;
typedef unsigned long long	uint64_t;

typedef int8_t		s8;
typedef int16_t		s16;
typedef int32_t		s32;
typedef int64_t		s64;

typedef uint8_t		u8;
typedef uint16_t	u16;
typedef uint32_t	u32;
typedef uint64_t	u64;



#endif
