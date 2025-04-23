#pragma once
#ifndef _SSLE_CORE_CLIENT_H_
#define _SSLE_CORE_CLIENT_H_



#include "../ssle_def.h"
#include <sle_ssap_client.h>

ssapc_write_param_t* sle_client_get_send_param(void);
void sle_client_core_init(void);
errcode_t sle_client_set_r_cb(sle_r_cb_t r_cb);



#endif // _SSLE_CORE_CLIENT_H_
