#include "ssle_server_msgqueue.h"
#include "../ssle_def.h"

#include "ddef.h"
#include "ttool.h"

#include "ssle_server_adv.h"
#include "ssle_server_core.h"

#include <pinctrl.h>
#include <securec.h>
#include <sle_device_discovery.h>
#include <sle_errcode.h>
#include <sle_low_latency.h>
#include <sle_ssap_server.h>
#include <soc_osal.h>



static unsigned long g_sle_server_msgqueue_id;



void sle_server_msgqueue_create(void) {
	s8 ret = osal_msg_queue_create(
		"sle_server_msgqueue", SLE_SERVER_MSG_QUEUE_LEN,
		&g_sle_server_msgqueue_id, 0, SLE_SERVER_MSG_QUEUE_MAX_SIZE
	);

	if (ret != OSAL_SUCCESS)
		ERROR("%s sle_server_create_msgqueue message queue create failed!\n", SLE_SERVER_LOG);
}


void sle_server_msgqueue_delete(void) {
	osal_msg_queue_delete(g_sle_server_msgqueue_id);
}


void sle_server_msgqueue_write(u8* buffer_addr, u16 buffer_size) {
	osal_msg_queue_write_copy(g_sle_server_msgqueue_id, (void*)buffer_addr, (u32)buffer_size, 0);
}


s32 sle_server_msgqueue_receive(u8* buffer_addr, u32* buffer_size) {
	return osal_msg_queue_read_copy(g_sle_server_msgqueue_id, (void*)buffer_addr, buffer_size, 1);//SLE_SERVER_QUEUE_DELAY);
	// 																						10ms/1
}
