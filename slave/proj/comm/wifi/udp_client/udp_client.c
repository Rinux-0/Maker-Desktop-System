#include "udp_client.h"

#include "ddef.h"
#include "ttool.h"

#include "lwip/nettool/misc.h"
#include "lwip/ip4_addr.h"
#include "lwip/netif.h"
#include "lwip/netifapi.h"
#include "lwip/sockets.h"

#include "cmsis_os2.h"
#include "app_init.h"
#include "soc_osal.h"
#include "wifi_connect.h"



static u8 wifi_server_ssid[24] = "Rinux";
static u8 wifi_server_pwd[24] = "87492324";
static u8 wifi_server_ip[24] = "192.168.6.49";
static u16 wifi_server_port = 8749;

static s32 sock_fd;        // 在sock_fd 进行监听，在 new_fd 接收新的链接
static c8 recvBuf[512];

// 服务器的地址信息
static struct sockaddr_in send_addr;
static socklen_t addr_length = sizeof(send_addr);



void udp_client_init(void) {
	// 连接Wifi
	wifi_connect((c8*)wifi_server_ssid, (c8*)wifi_server_pwd);

	// 创建socket
	// LOG("create socket start!\n");
	sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock_fd == -1) {
		ERROR("create socket failed!\n");
		return;
	}
	// DATA("create socket end!\n");

	// 初始化预连接的服务端地址
	send_addr.sin_family = AF_INET;
	send_addr.sin_port = htons(wifi_server_port);
	send_addr.sin_addr.s_addr = inet_addr((c8*)wifi_server_ip);
	addr_length = sizeof(send_addr);

	LOG("");
}


void udp_client_oneloop(void) {
	if (false == udp_is_conn())
		udp_client_init();

	bzero(recvBuf, sizeof(recvBuf));

	// 接收服务端返回的字符串
	recvfrom(sock_fd, recvBuf, sizeof(recvBuf), 0, (struct sockaddr*)&send_addr, &addr_length);
	DATA("%s:%d=>%s\n", inet_ntoa(send_addr.sin_addr), ntohs(send_addr.sin_port), recvBuf);
}


void udp_client_exit(void) {
	// 关闭此 socket
	lwip_close(sock_fd);
}


// 修改 wifi 配置
void udp_client_modify_wifi_cfg(u8* ssid, u8* pwd, u8* ip, u16 port) {
	strcpy((char*)wifi_server_ssid, (char*)ssid);
	strcpy((char*)wifi_server_pwd, (char*)pwd);
	strcpy((char*)wifi_server_ip, (char*)ip);
	wifi_server_port = port;

	LOG("");
}


void udp_client_send_data(const u8* data, u16 len) {
	// 发送数据到服务远端
	sendto(sock_fd, (c8*)data, len, 0, (struct sockaddr*)&send_addr, addr_length);
}
