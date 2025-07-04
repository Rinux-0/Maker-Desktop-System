#pragma once
#ifndef _TRINITY_DEF_H_
#define _TRINITY_DEF_H_



#if !defined(CONFIG_DEVICE_TRINITY_SOUND)
#	define sound_init()
#	define sound_oneloop()
#	define sound_exit()
#	define sound_entry()
#endif
#if !defined(CONFIG_DEVICE_TRINITY_VFD)
#	define vfd_init()
#	define vfd_oneloop()
#	define vfd_exit()
#	define vfd_entry()
#endif



#endif // _TRINITY_DEF_H_
