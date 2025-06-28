#pragma once
#ifndef _UDP_CLIENT_H_
#define _UDP_CLIENT_H_




void udp_client_init(void);
void udp_client_oneloop(void);
void udp_client_exit(void);

void udp_client_send_data(const char* data, int len);



#endif // _UDP_CLIENT_H_
