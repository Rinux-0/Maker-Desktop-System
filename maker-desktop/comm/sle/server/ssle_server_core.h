#pragma once
#ifndef _SSLE_CORE_SERVER_H_
#define _SSLE_CORE_SERVER_H_



#include "ddef.h"
#include "../ssle_def.h"
#include <sle_ssap_server.h>

typedef void (*sle_server_msg_queue)(u8* buffer_addr, u16 buffer_size);

errcode_t sle_server_send_report_by_uuid(sle_target_t target, const u8* data, u8 len);
errcode_t sle_server_send_report_by_hdl(sle_target_t target, const u8* data, u16 len);
u16 sle_server_is_client_connected(void);
void sle_server_register_msg(sle_server_msg_queue  sle_server_msg);
u16 sle_server_get_conn_id(void);
errcode_t sle_server_init_core(void);
errcode_t sle_server_set_r_cb(sle_r_cb_t r_cb);



#endif // _SSLE_CORE_SERVER_H_
