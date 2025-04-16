#pragma once
#ifndef _SSLE_CORE_CLIENT_H_
#define _SSLE_CORE_CLIENT_H_



#include "def.h"
#include "sle_ssap_client.h"

void sle_client_init(void);
void sle_client_start_scan(void);
ssapc_write_param_t* get_sle_client_send_param(void);



#endif // _SSLE_CORE_CLIENT_H_
