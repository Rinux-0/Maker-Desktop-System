#pragma once
#ifndef _HTTP_CORE_H_
#define _HTTP_CORE_H_



#include "ddef.h"
#include "lwip/sockets.h"

bool http_client_connect(struct sockaddr* send_addr, socklen_t addr_length);
bool http_client_get(c8* request_way, c8* url_path, bool is_conn_keep_alive, c8* data);
bool http_is_connected(void);


#endif // _HTTP_CORE_H_
