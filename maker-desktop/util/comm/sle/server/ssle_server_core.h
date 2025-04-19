#pragma once
#ifndef _SSLE_CORE_SERVER_H_
#define _SSLE_CORE_SERVER_H_


#include "def.h"
#include "errcode.h"
#include "sle_ssap_server.h"

typedef void (*sle_server_msg_queue)(u8* buffer_addr, u16 buffer_size);

errcode_t sle_server_send_report_by_uuid(const u8* data, u8 len);
errcode_t sle_server_send_report_by_handle(const u8* data, u16 len);
u16 sle_client_is_connected(void);
void sle_server_register_msg(sle_server_msg_queue  sle_server_msg);
u16 get_connect_id(void);




#include "../ssle_def.h"

errcode_t sle_server_core_init(void);
errcode_t sle_server_set_r_cb(sle_r_cb_t r_cb);




#endif // _SSLE_CORE_SERVER_H_
