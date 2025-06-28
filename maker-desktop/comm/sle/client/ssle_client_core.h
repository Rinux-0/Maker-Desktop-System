#pragma once
#ifndef _SSLE_CORE_CLIENT_H_
#define _SSLE_CORE_CLIENT_H_



#include "ddef.h"
#include "../ssle_def.h"
#include <sle_ssap_client.h>

// index-name
// 0-pc
// 1-dynamic
// 2-health
// 3-keyboard
// 4-receiver
// 5-trinity
// 6-keytest
//
// sle_client_conn_id [ index-name ] = conn_id (上电顺序)
extern s8 sle_client_conn_id[sle_target_max];

s8 sle_client_find_server_index_by_name(const u8* server_name);
s8 sle_client_find_server_index_by_conn_id(u16 conn_id);

ssapc_write_param_t* sle_client_get_send_param(void);
void sle_client_init_core(void);
errcode_t sle_client_set_r_cb(sle_r_cb_t r_cb);



#endif // _SSLE_CORE_CLIENT_H_
