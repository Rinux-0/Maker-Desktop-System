#include "ssle.h"
#include "ssle_def.h"

#include <soc_osal.h>
#if defined(CONFIG_COMM_SLE_CLIENT)
#	include "client/ssle_client.h"
#elif defined(CONFIG_COMM_SLE_SERVER)
#	include "server/ssle_server.h"
#endif



void sle_entry(void) {
	osal_task* task_handle = NULL;

	osal_kthread_lock();

	task_handle = osal_kthread_create(
		#if defined(CONFIG_COMM_SLE_CLIENT)
		(osal_kthread_handler)sle_client_task,
		0, "SLEUartDongleTask",
		#elif defined(CONFIG_COMM_SLE_SERVER)
		(osal_kthread_handler)sle_server_task,
		0, "SLEUartServerTask",
		#endif
		SLE_TASK_STACK_SIZE
	);

	if (task_handle != NULL) {
		osal_kthread_set_priority(
			task_handle, SLE_TASK_PRIO
		);
	} else {
		while (1) {
			osal_printk("[1]fail\r\n");
		}
	}

	osal_kthread_unlock();
}
