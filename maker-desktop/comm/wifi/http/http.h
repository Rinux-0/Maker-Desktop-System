#pragma once
#ifndef _HTTP_H_
#define _HTTP_H_



#include "ddef.h"
#include <lwip/sockets.h>

void http_init(struct sockaddr* send_addr, socklen_t addr_length, u8* ip, u16 port);
void http_oneloop(void);
void http_exit(void);

void http_send(c8* request_way, c8* url_path, bool is_conn_keep_alive, c8* data);



#endif // _HTTP_H_
