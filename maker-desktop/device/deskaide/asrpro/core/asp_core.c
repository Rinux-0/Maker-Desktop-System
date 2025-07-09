#include "asp_core.h"

#include "ddef.h"
#include "ttool.h"

#include "ssle.h"
#include "uuart.h"



static volatile bool is_wating;



/******************************* 不得已的封装破坏 *******************************/
/**/void sound_uart_r_int_handler(const void* buffer, u16 length, bool error);/**/
/**/void vfd_uart_r_int_handler(const void* buffer, u16 length, bool error);  /**/
/******************************* 不得已的封装破坏 *******************************/



static bool asp_core_process_cmd(c8* sub_cmd, u8 sub_len) {
	c8* tmp;

	if ((tmp = strnstr(sub_cmd, "snd", sub_len))) {
		DATA("snd ");

		sound_uart_r_int_handler((void*)sub_cmd, sub_len, false);
	} else if ((tmp = strnstr(sub_cmd, "lamp", sub_len))) {
		DATA("lamp ");

		sle_write(health, (u8*)sub_cmd, sub_len);
	} else if ((tmp = strnstr(sub_cmd, "vfd", sub_len))) {
		DATA("vfd ");

		vfd_uart_r_int_handler((void*)sub_cmd, sub_len, false);
	} else if ((tmp = strnstr(sub_cmd, "...", sub_len))) {
		DATA("... ]\n\n");
	} else {
		return false;
	}

	DATA("]\n\n");
	return true;
}


void asp_cmd_entry(c8* buff, u16 length) {
	if (is_wating) {
		LOG("\n\tLast asp cmd is processing ...\n\n");
		return;
	}

	is_wating = true;

	c8* substr = strnstr(buff, "asp", length);
	if (substr == NULL) {
		ERROR("\n\tcmd error: [%s]\n\n", buff);
		is_wating = false;
		return;
	}
	DATA("\n\tGet asp cmd[%u]: [ ", length);
	substr += sizeof("asp");
	length -= sizeof("asp");

	if (false == asp_core_process_cmd(substr, length))
		DATA("ERROR... ]\n\n");

	is_wating = false;
}
