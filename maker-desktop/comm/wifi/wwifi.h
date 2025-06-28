#pragma once
#ifndef _WWIFI_H_
#define _WWIFI_H_



#include "ddef.h"
#include "wwifi_def.h"

void wifi__init(void);
void wifi_oneloop(void);
void wifi_exit(void);

void wifi_write(const u8* data, u16 len);



#endif // _WWIFI_H_
