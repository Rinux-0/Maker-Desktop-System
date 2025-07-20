#pragma once
#ifndef _UDP_CLIENT_H_
#define _UDP_CLIENT_H_



#include "ddef.h"

void udp_client_init(void);
void udp_client_oneloop(void);
void udp_client_exit(void);

void udp_client_modify_wifi_cfg(u8* ssid, u8* pwd, u8* ip, u16 port);
void udp_client_send_data(const u8* data, u16 len);



#endif // _UDP_CLIENT_H_
