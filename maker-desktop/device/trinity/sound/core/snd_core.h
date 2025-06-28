#pragma once
#ifndef _SND_CORE_H_
#define _SND_CORE_H_



#include "ddef.h"

void snd_core_init_pin(void);
bool snd_core_process_cmd(c8* sub_cmd, u8 sub_len);



#endif // _SND_CORE_H_
