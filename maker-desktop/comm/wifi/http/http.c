#include "http.h"

#include "ddef.h"
#include "ttool.h"

#include "http_core.h"

#include <lwip/sockets.h>



static struct sockaddr* sock_addr;
static socklen_t sock_addr_length;



// // 修改 wifi 配置
// void udp_client_modify_wifi_cfg(u8* ssid, u8* pwd, u8* ip, u16 port) {
// 	strcpy((char*)wifi_server_ssid, (char*)ssid);
// 	strcpy((char*)wifi_server_pwd, (char*)pwd);
// 	strcpy((char*)wifi_server_ip, (char*)ip);
// 	wifi_server_port = port;

// 	LOG("");
// }


// void udp_client_send_data(const u8* data, u16 len) {
// 	// 发送数据到服务远端
// 	sendto(sock_fd, (c8*)data, len, 0, (struct sockaddr*)&send_addr, addr_length);
// }



void http_init(struct sockaddr* send_addr, socklen_t addr_length, u8* ip, u16 port) {
	unused(ip);
	unused(port);

	if (http_client_connect(send_addr, addr_length)) {
		sock_addr = send_addr;
		sock_addr_length = addr_length;
	}
}


void http_oneloop(void) {
	if (http_is_connected() == false && sock_addr && sock_addr_length)
		http_client_connect(sock_addr, sock_addr_length);
}


void http_exit(void) {
	// lwip_close(socket_id);
}


void http_send(c8* request_way, c8* url_path, bool is_conn_keep_alive, c8* data) {
	// http_client_connect();
	http_client_get(request_way, url_path, is_conn_keep_alive, data);
}
