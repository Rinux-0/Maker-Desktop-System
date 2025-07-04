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

#define CONFIG_WIFI_SSID            "Rinux"//"kiiye9697"                          // 要连接的WiFi 热点账号
#define CONFIG_WIFI_PWD             "87492324"//"2252562878"                         // 要连接的WiFi 热点密码
#define CONFIG_SERVER_IP            "10.5.173.3"//"192.168.225.100"                     // 要连接的服务器IP
#define CONFIG_SERVER_PORT          8888                                 // 要连接的服务器端口



int sock_fd;        // 在sock_fd 进行监听，在 new_fd 接收新的链接

// 服务器的地址信息
struct sockaddr_in send_addr;
socklen_t addr_length = sizeof(send_addr);
char recvBuf[512];

char send_data[] = "nfc:0; finger:1; rate:2; temp:3; distance:4\n";



void udp_client_init(void) {
    // 连接Wifi
    wifi_connect(CONFIG_WIFI_SSID, CONFIG_WIFI_PWD);

    // 创建socket
    DATA("create socket start!\n");
    if ((sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        DATA("create socket failed!\n");
        return;
    }
    DATA("create socket end!\n");
    // 初始化预连接的服务端地址
    send_addr.sin_family = AF_INET;
    send_addr.sin_port = htons(CONFIG_SERVER_PORT);
    send_addr.sin_addr.s_addr = inet_addr(CONFIG_SERVER_IP);
    addr_length = sizeof(send_addr);
}


void udp_client_oneloop(void) {
    bzero(recvBuf, sizeof(recvBuf));

    // 发送数据到服务远端
    DATA("sendto start!\n");
    sendto(sock_fd, (const char*)send_data, strlen(send_data), 0, (struct sockaddr*)&send_addr, addr_length);
    DATA("sendto end!\n");
    // 线程休眠一段时间
    osDelay(100);

    // 接收服务端返回的字符串
    recvfrom(sock_fd, recvBuf, sizeof(recvBuf), 0, (struct sockaddr*)&send_addr, &addr_length);
    DATA("%s:%d=>%s\n", inet_ntoa(send_addr.sin_addr), ntohs(send_addr.sin_port), recvBuf);
}


void udp_client_exit(void) {
    // 关闭这个 socket
    lwip_close(sock_fd);
}


void udp_client_send_data(const u8* data, u16 len) {
    sendto(sock_fd, (char*)data, len, 0, (struct sockaddr*)&send_addr, addr_length);
    // sendto(sock_fd, send_data, strlen(send_data), 0, (struct sockaddr*)&send_addr, addr_length);
}
