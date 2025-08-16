#include "kpd_core.h"
#include "keypad_def.h"

#include "ddef.h"
#include "ttool.h"

#include "comm.h"
#include "color.h"
#include "hhid.h"
#include "ssle.h"
#include "uuart.h"

#include <non_os_reboot.h>
#include <gpio.h>
#include <pinctrl.h>



// static hid_pack_t hid_wp = { 0 };
static hid_pack_t* kpd_hid_pack = NULL;
// static hid_pack_t other_hid_pack = { 0 };

// [0]up [1]down
static u8 kpd_status_past[KPD_NUM_REGISTER] = {};
static u8 kpd_status_now[KPD_NUM_REGISTER] = {};

static const s16 kpd_keymap[KPD_NUM_REGISTER * 8] = {
	//     0            1               2               3           4           5           6           7
	/* 0 */Reserved,	Reserved,		Reserved,		Fn,			PrtSc,		F5,			App,		Lock_PadNum,
	/* 1 */Pad_Divide,	Pad_Multiply,	Pad_Subtract,	Pad_Num_7,	Pad_Num_8,	Pad_Num_9,	Pad_Add,	Pad_Num_4,
	/* 2 */Pad_Num_5,	Pad_Num_6,		Pad_Num_1,		Pad_Num_2,	Pad_Num_3,	Pad_Enter,	Pad_Num_0,	Pad_Dot
};



void kpd_init_pin(void) {	// 都初始化为 GPIO模式
	// CE
	uapi_pin_set_mode(KPD_PIN_74HC165_CE, PIN_MODE_0);
	uapi_gpio_set_dir(KPD_PIN_74HC165_CE, GPIO_DIRECTION_OUTPUT);
	uapi_gpio_set_val(KPD_PIN_74HC165_CE, GPIO_LEVEL_LOW);

	// CP (CLK-手动)
	uapi_pin_set_mode(KPD_PIN_74HC165_CP, PIN_MODE_0);
	uapi_gpio_set_dir(KPD_PIN_74HC165_CP, GPIO_DIRECTION_OUTPUT);
	uapi_gpio_set_val(KPD_PIN_74HC165_CP, GPIO_LEVEL_LOW);

	// PL
	uapi_pin_set_mode(KPD_PIN_74HC165_PL, PIN_MODE_0);
	uapi_gpio_set_dir(KPD_PIN_74HC165_PL, GPIO_DIRECTION_OUTPUT);
	uapi_gpio_set_val(KPD_PIN_74HC165_PL, GPIO_LEVEL_HIGH);

	// Q7 (MISO)
	uapi_pin_set_mode(KPD_PIN_74HC165_Q7, PIN_MODE_0);
	uapi_gpio_set_dir(KPD_PIN_74HC165_Q7, GPIO_DIRECTION_INPUT);

	// LED_SLE
	uapi_pin_set_mode(LED_PIN_SLE, PIN_MODE_0);
	uapi_gpio_set_dir(LED_PIN_SLE, GPIO_DIRECTION_OUTPUT);
	uapi_gpio_set_val(LED_PIN_SLE, GPIO_LEVEL_HIGH);
}


// 收到 other
// static void kpd_uart_r_int_handler(const void* buffer, u16 length, bool error) {
// 	unused(error);

// 	u8* buff_rx = (u8*)buffer;
// 	u8* p_other = (u8*)&other_hid_pack;

// 	LOG("length: %d\n", length);

// 	for (u16 i = 0; i < length; i++)
// 		p_other[i] = buff_rx[i];

// 	// DATA("\t[d0]%02x [d1]%02x [d2]%02x [d3]%02x [d4]%02x [d5]%02x [d6]%02x [d7]%02x\n" "\t[s]%02x\n",
// 	// 	p_other[5], p_other[6], p_other[7], p_other[8], p_other[9], p_other[10], p_other[11], p_other[12], p_other[13]
// 	// );

// 	// if (kpd_merge_hid_wp())
// 	kpd_send_hid_wp(NULL);
// }


void kpd_init_int_cb(void) {
	// 来自other
	// uart_set_r_cb(UART_BUS_ID(1), kpd_uart_r_int_handler);
}


void kpd_update_past(void) {
	// 键态(现) ---> 键态(过)
	for (u8 i = 0; i < KPD_NUM_REGISTER; i++)
		kpd_status_past[i] = kpd_status_now[i];
}


void kpd_read_now(void) {
	// 并行存入
	tool_pin_gpio_refresh_u(KPD_PIN_74HC165_PL, 1);

	// 串行取出
	for (u8 i = 0; i < KPD_NUM_REGISTER; i++) {		// 逐寄存器
		u8 byte = 0;
		for (u8 j = 0; j < 8; j++) {				// 逐bit
			byte |= !uapi_gpio_get_val(KPD_PIN_74HC165_Q7) << j;	// 01反转
			tool_pin_gpio_refresh_u(KPD_PIN_74HC165_CP, 1);
		}
		kpd_status_now[i] = byte;
	}
}


bool kpd_is_fn_pressed(void) {
	return kpd_status_now[0] & (1 << 3);
}


static void kpd_set_media_hid_wp(u32 keys) {
	kpd_hid_pack = (hid_pack_t*)hid_set_wp(
#		if defined(CONFIG_COMM_FORMAT_HID_XXX)
#		elif defined(CONFIG_COMM_FORMAT_HID_CH9329)
		HID_CH9329_CMD_SEND_KB_MEDIA_DATA,
#		endif
		3, (u8*)&keys, NULL
	);
}


/// @note 假定 Fn 已按下
void kpd_fn_processer(void) {
	u32 media_keys = 0;
	if (kpd_status_now[0] & (1 << 4)) {			// PrtSc	-Volume_Down
		media_keys |= Volume_Down;
	} else if (kpd_status_now[0] & (1 << 5)) {	// F5		-Volume_Mute
		media_keys |= Volume_Mute;
	} else if (kpd_status_now[0] & (1 << 6)) {	// App		-Volume_Up
		media_keys |= Volume_Up;
	}
	kpd_set_media_hid_wp(media_keys);
	// if (kpd_merge_hid_wp())
	kpd_send_hid_wp(NULL);

	if (kpd_status_now[2] & (1 << 6)) {		// 0		- 灯光 切换
		color_set_mode_next();
		LOG("");
	} if (kpd_status_now[2] & (1 << 1)) {	// 6		- color_hsv_h_speed +
		color_ctrl_hsv_h(+100);
	} if (kpd_status_now[1] & (1 << 7)) {	// 4		- color_hsv_h_speed -
		color_ctrl_hsv_h(-100);
	} if (kpd_status_now[1] & (1 << 4)) {	// 8		- color_hsv_v_max +
		color_ctrl_hsv_v(false, +1.f, 0b00);
	} if (kpd_status_now[2] & (1 << 3)) {	// 2		- color_hsv_v_max -
		color_ctrl_hsv_v(false, -1.f, 0b00);
	} if (kpd_status_now[1] & (1 << 0)) {	// /		- color_hsv_v_is_changing 切换
		color_ctrl_hsv_v(true, 0.f, 0b00);
	}

	if (kpd_status_now[2] & (1 << 5)) {		// Enter	- comm_way 切换
		if (comm_way++ == COMM_WAY_SLE)
			comm_way = COMM_WAY_UART;
		LOG("");
	}

	if (kpd_status_now[2] & (1 << 7)) {		// .		- 软复位
		reboot_system(REBOOT_CAUSE_BT_WATCHDOG);
	}

	LOG("comm_way: %d\ncolor_mode: %d\n", comm_way, color_get_mode());
}


/// @brief 不处理，直接对比键态
static bool kpd_is_diff(void) {
	for (u8 i = 0; i < KPD_NUM_REGISTER; i++)
		if (kpd_status_past[i] != kpd_status_now[i])
			return true;

	return false;
}


/// @brief 消抖处理后，再次对比键态
bool kpd_is_valid_diff(void) {
	if (kpd_is_diff()) {
		kpd_update_past();
		tool_delay_u_nop(100);	// 抖动
		kpd_read_now();

		if (!kpd_is_diff())
			return true;
	}

	return false;
}


void kpd_set_kpd_hid_wp(void) {
	kpd_hid_pack = (hid_pack_t*)hid_set_wp(
#		if defined(CONFIG_COMM_FORMAT_HID_XXX)
#		elif defined(CONFIG_COMM_FORMAT_HID_CH9329)
		HID_CH9329_CMD_SEND_KB_GENERAL_DATA,
#		endif
		KPD_NUM_REGISTER,
		kpd_status_now,
		kpd_keymap
	);
}


static void kpd_uart_write_hid_wp(const hid_pack_t* wp) {
	uart_write(
		UART_BUS_ID(1),
		(const u8*)wp,
#		if defined(CONFIG_COMM_FORMAT_HID_XXX)
#		elif defined(CONFIG_COMM_FORMAT_HID_CH9329)
		// hid_wp.length + 6
		kpd_hid_pack->length + 6
#		endif
	);
}


static void kpd_sle_write_hid_wp(const hid_pack_t* wp) {
	sle_write(
		pc,
		(const u8*)wp,
#		if defined(CONFIG_COMM_FORMAT_HID_XXX)
#		elif defined(CONFIG_COMM_FORMAT_HID_CH9329)
		// hid_wp.length + 6
		kpd_hid_pack->length + 6
#		endif
	);
}


// bool kpd_merge_hid_wp(void) {
// 	// 以 kpd 为主，不考虑重复键
// 	if (kpd_hid_pack == NULL) {
// 		hid_wp = other_hid_pack;
// 		return true;
// 	}

// #	if defined(CONFIG_COMM_FORMAT_HID_XXX)
// #	elif defined(CONFIG_COMM_FORMAT_HID_CH9329)
// 	if (kpd_hid_pack->cmd != other_hid_pack.cmd) {
// 		kpd_send_hid_wp(&other_hid_pack);
// 		return false;
// 	}

// 	hid_wp = *kpd_hid_pack;
// 	if (kpd_hid_pack->cmd == HID_CH9329_CMD_SEND_KB_GENERAL_DATA) {
// 		u8 i = 2, j = 2;
// 		for (; i < 8 && kpd_hid_pack->data[i]; i++);
// 		while (i < 8 && j < 8 && other_hid_pack.data[j])
// 			hid_wp.data[i++] = (u8)other_hid_pack.data[j++];	// (u8) 省略会出错

// 		if (other_hid_pack.data[8] > 0)
// 			hid_wp.data[8] += other_hid_pack.data[8] - 0x0C;
// 	} else if (kpd_hid_pack->cmd == HID_CH9329_CMD_SEND_KB_MEDIA_DATA) {
// 		u8 i = 1, j = 1;
// 		for (; i < 4 && kpd_hid_pack->data[i]; i++);
// 		while (i < 4 && j < 4 && other_hid_pack.data[j])
// 			hid_wp.data[i++] = (u8)other_hid_pack.data[j++];	// (u8) 省略会出错

// 		if (other_hid_pack.data[4] > 0)
// 			hid_wp.data[4] += other_hid_pack.data[4] - 0x0B;
// 	}
// #	endif

// 	return true;
// }


void kpd_send_hid_wp(const hid_pack_t* wp) {
	if (wp == NULL)
		wp = kpd_hid_pack;
		// wp = &hid_wp;

	switch (comm_way) {
	default:					comm_way = COMM_WAY_SLE;
	/****/case COMM_WAY_SLE:	kpd_sle_write_hid_wp(wp);
	break;case COMM_WAY_UART:	kpd_uart_write_hid_wp(wp);
	}
	// u8* d=(u8*)wp;
	// DATA("\n\t%d | %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x\n\n",
	// 	uapi_pin_get_mode(15),
	// 	d[0], d[1], d[2], d[3], d[4], d[5], d[6],
	// 	d[7], d[8], d[9], d[10], d[11], d[12], d[13]
	// );
}
