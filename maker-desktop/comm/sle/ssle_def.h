#pragma once
#ifndef _SSLE_DEF_H_
#define _SSLE_DEF_H_



#if !defined(CONFIG_COMM_SLE_CLIENT) && !defined(CONFIG_COMM_SLE_SERVER)
#	define CONFIG_COMM_SLE_CLIENT	// 方便编码（*）
// #	define CONFIG_COMM_SLE_SERVER	// 方便编码
#endif



#include "ddef.h"

#define SLE_TASK_PRIO					(28)
// #define SLE_TASK_DURATION_MS			(2000)
/// SLE_core
#define SLE_UUID_16BIT_LEN				(2)
#define SLE_UUID_128BIT_LEN				(16)

#define SLE_CONN_IDENTITY				"MDT_SLE_server"
#define SLE_COMM_SHIT					"HUAWEI M-Pencil 3"



#if defined(CONFIG_COMM_SLE_CLIENT)
#	include <sle_ssap_client.h>
typedef ssapc_handle_value_t ssle_ssap_value_t;
/// SLE_core_client
#	define SLE_MTU_SIZE					(520)
#	define SLE_SEEK_INTERVAL			(100)
#	define SLE_SEEK_WINDOW				(100)
// #	define SLE_TASK_DELAY_MS			(1000)
// #	define SLE_WAIT_SLE_CORE_READY_MS	(5000)
// #	define SLE_RECV_CNT					(1000)
// #	define SLE_LOW_LATENCY_2K			(2000)
#	define SLE_CLIENT_MAX_CONN			(8)
#	define SLE_CLIENT_LOG				"[sle client]"
#elif defined(CONFIG_COMM_SLE_SERVER)
#	include <sle_ssap_server.h>
typedef ssaps_req_write_cb_t ssle_ssap_value_t;
// #	define SLE_SERVER_DELAY_COUNT		(5)
#	define SLE_ADV_HANDLE				(1)
#	define SLE_SERVER_MSG_QUEUE_LEN		(5)
#	define SLE_SERVER_MSG_QUEUE_MAX_SIZE (32)
// #	define SLE_SERVER_QUEUE_DELAY		(0xFFFFFFFF)
// #	define SLE_SERVER_BUFF_MAX_SIZE		(800)
#	define SLE_SERVER_LOG				"[sle server]"
/// SLE_server_core
#	define SLE_UUID_SERVER_SERVICE		(0x2222)
#	define SLE_UUID_SERVER_NTF_REPORT	(0x2323)
#	define SLE_UUID_TEST_PROPERTIES		(SSAP_PERMISSION_READ | SSAP_PERMISSION_WRITE)
// #	define SLE_UUID_TEST_OPERATION_INDICATION  (SSAP_OPERATE_INDICATION_BIT_READ | SSAP_OPERATE_INDICATION_BIT_WRITE)
#	define SLE_UUID_TEST_DESCRIPTOR		(SSAP_PERMISSION_READ | SSAP_PERMISSION_WRITE)
#	define SLE_OCTET_BIT_LEN			(8)
#	define SLE_UUID_LEN_2				(2)
#	define SLE_UUID_INDEX				(14)
#	define SLE_BT_INDEX_4				(4)
#	define SLE_BT_INDEX_0				(0)
#	define SLE_UART_BUFF_LENGTH			(0x100)
// #	define SLE_SERVER_INIT_DELAY_MS		(1000)
/// SLE_server_core_adv
#	define SLE_NAME_MAX_LENGTH			(16)		// sle device/test name
#	define SLE_CONN_INTV_MIN			(0x64)		// 连接调度间隔12.5ms，单位125us
#	define SLE_CONN_INTV_MAX			(0x64)		// 连接调度间隔12.5ms，单位125us
#	define SLE_ADV_INTERVAL_MIN			(0xC8)		// 连接调度间隔25ms，单位125us
#	define SLE_ADV_INTERVAL_MAX			(0xC8)		// 连接调度间隔25ms，单位125us
#	define SLE_CONN_SUPERVISION_TIMEOUT	(0x1F4)		// 超时时间5000ms，单位10ms
#	define SLE_CONN_MAX_LATENCY			(0x1F3)		// 超时时间4990ms，单位10ms
#	define SLE_ADV_TX_POWER				(10)		// 广播发送功率
#	define SLE_ADV_DATA_LEN_MAX			(251)		// 最大广播数据长度
#endif

typedef void (*sle_r_cb_t)(u8 cs_id, u16 conn_id, ssle_ssap_value_t* read_cb_para, errcode_t status);

// 0-pc
// 1-dynamic
// 2-health
// 3-keyboard
// 4-receiver
// 5-trinity
// 6-keytest
// 7-tmptest
typedef enum {
	pc, dynamic, health, keyboard, receiver, trinity, keytest, tmptest,
	sle_target_max
} sle_target_t;



#endif // _SSLE_DEF_H_
