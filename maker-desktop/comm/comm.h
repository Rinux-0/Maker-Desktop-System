#pragma once
#ifndef _COMM_H_
#define _COMM_H_



#include "comm_def.h"

extern comm_way_t comm_w_way;

void comm_init(void);
void comm_oneloop(void);
void comm_exit(void);



#endif // _COMM_H_
