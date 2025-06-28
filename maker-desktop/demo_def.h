#pragma once
#ifndef _DEMO_DEF_H_
#define _DEMO_DEF_H_



#if defined(CONFIG_DEVICE)
#	define DEMO_NAME device
#	define DEV_OR_TEST DEVICE
#elif defined(CONFIG_TEST)
#	define DEMO_NAME test
#	define DEV_OR_TEST TEST
#else
#	error "CONFIG_DEVICE or CONFIG_TEST isn't defined!"
#endif



#if !defined(CONFIG_COMM_SPI)
#	define color_init()
#	define color_oneloop()
#	define color_exit()
#endif



#endif // _DEMO_DEF_H_
