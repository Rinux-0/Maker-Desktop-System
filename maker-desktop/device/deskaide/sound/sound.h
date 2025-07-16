#pragma once
#ifndef _SOUND_H_
#define _SOUND_H_



#include "ddef.h"
#include "ssle.h"

void sound_sle_r_int_handler(u8 cs_id, u16 conn_id, ssle_ssap_value_t* read_cb_para, errcode_t status);

// void sound_init(void);
// void sound_oneloop(void);
// void sound_exit(void);
void sound_entry(void);



#endif // _SOUND_H_

/*
	snd bt onf

	snd play onf

	snd phone onf
	snd phone off

	snd volume +
	snd volume -

	snd chapter +
	snd chapter -
*/
