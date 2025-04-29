#pragma once
#ifndef _SSLE_CLIENT_CONN_H_
#define _SSLE_CLIENT_CONN_H_



#include "ddef.h"

typedef struct {
	u16 conn_id[8];
	u8  name[8][16];
}sle_conn_map_t;



#endif // _SSLE_CLIENT_CONN_H_
