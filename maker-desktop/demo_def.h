#pragma once
#ifndef _DEMO_DEF_H_
#define _DEMO_DEF_H_



#if defined(CONFIG_DEVICE)
#	define DEV_OR_TEST device
#elif defined(CONFIG_TEST)
#	define DEV_OR_TEST test
#else
#	error "CONFIG_DEVICE or CONFIG_TEST isn't defined!"
#endif



#endif // _DEMO_DEF_H_
