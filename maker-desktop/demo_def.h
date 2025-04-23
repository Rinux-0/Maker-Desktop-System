#pragma once
#ifndef _DEMO_DEF_H_
#define _DEMO_DEF_H_



#if !defined(CONFIG_DEVICE)
#	define device_init()
#	define device_oneloop()
#	define device_exit()
#endif
#if !defined(CONFIG_HEALTH)
#	define health_init()
#	define health_oneloop()
#	define health_exit()
#endif



#endif // _DEMO_DEF_H_
