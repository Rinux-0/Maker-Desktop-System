#pragma once
#ifndef _WWIFI_H_
#define _WWIFI_H_



#include "ddef.h"
#include "wwifi_def.h"

// void wifi__init(void);
// void wifi_oneloop(void);
// void wifi_exit(void);
void wifi_entry(void);

void wifi_modify_cfg(u8* wifi_ssid, u8* wifi_pwd, u8* wifi_ip, s32 wifi_port);
void wifi_write(c8* request_way, c8* url_path, bool is_conn_keep_alive, c8* data);
// void wifi_write(const u8* data, u16 len);



#endif // _WWIFI_H_
