#include "http_core.h"

#include "ddef.h"
#include "ttool.h"

#include "lwip/netdb.h"
#include "lwip/sockets.h"



static int socket_id;

static const c8* fmt_request = (
	"%s %s HTTP/1.1\r\n"
	"content-type: application/json\r\n"
	"accept: application/json\r\n"
	"host: 192.168.65.49:8749\r\n"
	"connection: %s\r\n"
	"\r\n"
	"%s"
);

static c8 request[256];
static c8 recv_buf[64];

static bool http_is_conn;



bool http_client_connect(struct sockaddr* send_addr, socklen_t addr_length) {
	http_is_conn = false;

	socket_id = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_id < 0)
		return false;

	// LOG("[http]: allocated socket");

	if (connect(socket_id, send_addr, addr_length) != 0) {
		ERROR("[http]: socket connect failed errno=%d", errno);
		lwip_close(socket_id);
		return false;
	}

	DATA("[http]: connected");

	http_is_conn = true;
	return true;
}


/// @note 收消息会阻塞，暂只适用于 即发即收
bool http_client_get(c8* request_way, c8* url_path, bool is_conn_keep_alive, c8* data) {
	if (http_is_conn == false) {
		ERROR("http isn't connected");
		return false;
	}

	c8* request_conn;
	if (is_conn_keep_alive) {
		request_conn = "keep-alive";
	} else {
		request_conn = "close";
	}

	sprintf(request, fmt_request,
		request_way, url_path, request_conn, data
	);

	DATA(request);

	if (lwip_write(socket_id, request, strlen(request)) < 0) {
		lwip_close(socket_id);
		http_is_conn = false;
		return false;
	}

	LOG("[http]: socket send success");

	struct timeval receiving_timeout = {
		.tv_sec = 2,
		.tv_usec = 0
	};

	if (setsockopt(socket_id, SOL_SOCKET, SO_RCVTIMEO, &receiving_timeout, sizeof(receiving_timeout)) < 0) {
		ERROR("[http]: failed to set socket receiving timeout");
		lwip_close(socket_id);
		http_is_conn = false;
		return false;
	}

	// LOG("[http]: set socket receiving timeout success");

	// Read HTTP response
	// int r = 1;
	// while (r > 0) {
	// 	(void)memset_s(recv_buf, sizeof(recv_buf), 0, sizeof(recv_buf));
	// 	int r = lwip_read(socket_id, recv_buf, sizeof(recv_buf) - 1);
	// 	LOG(recv_buf);
	// }

	// LOG("[http]: done reading from socket. Last read return=%d errno=%d\r\n", r, errno);

	return true;
}


bool http_is_connected(void) {
	return http_is_conn;
}
