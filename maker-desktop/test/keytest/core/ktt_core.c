#include "ktt_core.h"
#include "keytest_def.h"

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



static hid_pack_t hid_wp = { 0 };
static hid_pack_t* ktt_hid_pack = NULL;
static hid_pack_t other_hid_pack = { 0 };

// [0]up [1]down
static u8 ktt_status_past[KTT_NUM_REGISTER] = {};
static u8 ktt_status_now[KTT_NUM_REGISTER] = {};

static const s16 ktt_keymap[KTT_NUM_REGISTER * 8] = {
	Fn, B, C, D,
	E, F, G, H,

	I, J, K, L,
	M, N, O, P
};



void ktt_init_pin(void) {	// 都初始化为 GPIO模式
	// CE
	uapi_pin_set_mode(KTT_PIN_74HC165_CE, PIN_MODE_0);
	uapi_gpio_set_dir(KTT_PIN_74HC165_CE, GPIO_DIRECTION_OUTPUT);
	uapi_gpio_set_val(KTT_PIN_74HC165_CE, GPIO_LEVEL_LOW);

	// CP (CLK-手动)
	uapi_pin_set_mode(KTT_PIN_74HC165_CP, PIN_MODE_0);
	uapi_gpio_set_dir(KTT_PIN_74HC165_CP, GPIO_DIRECTION_OUTPUT);
	uapi_gpio_set_val(KTT_PIN_74HC165_CP, GPIO_LEVEL_LOW);

	// PL
	uapi_pin_set_mode(KTT_PIN_74HC165_PL, PIN_MODE_0);
	uapi_gpio_set_dir(KTT_PIN_74HC165_PL, GPIO_DIRECTION_OUTPUT);
	uapi_gpio_set_val(KTT_PIN_74HC165_PL, GPIO_LEVEL_HIGH);

	// Q7 (MISO)
	uapi_pin_set_mode(KTT_PIN_74HC165_Q7, PIN_MODE_0);
	uapi_gpio_set_dir(KTT_PIN_74HC165_Q7, GPIO_DIRECTION_INPUT);

	// LED_SLE
	uapi_pin_set_mode(LED_PIN_SLE, PIN_MODE_0);
	uapi_gpio_set_dir(LED_PIN_SLE, GPIO_DIRECTION_OUTPUT);
	uapi_gpio_set_val(LED_PIN_SLE, GPIO_LEVEL_HIGH);
}


// 收到 other
static void ktt_uart_r_int_handler(const void* buffer, u16 length, bool error) {
	unused(error);

	u8* buff_rx = (u8*)buffer;
	u8* p_other = (u8*)&other_hid_pack;

	LOG("length: %d\n", length);

	for (u16 i = 0; i < length; i++)
		p_other[i] = buff_rx[i];

	// DATA("\t[d0]%02x [d1]%02x [d2]%02x [d3]%02x [d4]%02x [d5]%02x [d6]%02x [d7]%02x\n" "\t[s]%02x\n",
	// 	p_other[5], p_other[6], p_other[7], p_other[8], p_other[9], p_other[10], p_other[11], p_other[12], p_other[13]
	// );

	ktt_send_hid_wp();
}


void ktt_init_int_cb(void) {
	// 来自other
	uart_set_r_cb(UART_BUS_ID(1), ktt_uart_r_int_handler);
}


void ktt_update_past(void) {
	// 键态(现) ---> 键态(过)
	for (u8 i = 0; i < KTT_NUM_REGISTER; i++)
		ktt_status_past[i] = ktt_status_now[i];
}


void ktt_read_now(void) {
	// 并行存入
	tool_pin_gpio_refresh_u(KTT_PIN_74HC165_PL, 1);

	// 串行取出
	for (u8 i = 0; i < KTT_NUM_REGISTER; i++) {		// 逐寄存器
		u8 byte = 0;
		for (u8 j = 0; j < 8; j++) {				// 逐bit
			byte |= !uapi_gpio_get_val(KTT_PIN_74HC165_Q7) << j;	// 01反转
			tool_pin_gpio_refresh_u(KTT_PIN_74HC165_CP, 1);
		}
		ktt_status_now[i] = byte;
	}
}


bool ktt_is_fn_pressed(void) {
	return ktt_status_now[0] & (1 << 0);
}


/// @note 假定 Fn 已按下
void ktt_fn_processer(void) {
	if (ktt_status_now[0] & (1 << 1)) {	// B		- 灯光 切换
		color_set_mode_next();
		LOG("");
	}if (ktt_status_now[0] & (1 << 5)) {	// F		- color_hsv_h_speed 切换
		color_ctrl_hsv_h(0);
	} if (ktt_status_now[0] & (1 << 6)) {	// G		- color_hsv_s_is_changing 切换
		color_ctrl_hsv_s(true, false, 0b00);
	} if (ktt_status_now[1] & (1 << 2)) {	// K		- color_hsv_s_is_full 切换
		color_ctrl_hsv_s(false, true, 0b00);
	} if (ktt_status_now[0] & (1 << 7)) {	// H		- color_hsv_v_is_changing 切换
		color_ctrl_hsv_v(true, false, 0b00);
	} if (ktt_status_now[1] & (1 << 3)) {	// L		- color_hsv_v_is_full 切换
		color_ctrl_hsv_v(false, true, 0b00);
	}

	if (ktt_status_now[0] & (1 << 2)) {	// C		- comm_way 切换
		if (comm_way++ == COMM_WAY_SLE)
			comm_way = COMM_WAY_UART;
		LOG("");
	}

	if (ktt_status_now[0] & (1 << 3)) {	// D		- 软复位
		reboot_system(REBOOT_CAUSE_UNKNOWN);
	}



	LOG("comm_way: %d\ncolor_mode: %d\n", comm_way, color_get_mode());
}


/// @brief 不处理，直接对比键态
static bool ktt_is_diff(void) {
	for (u8 i = 0; i < KTT_NUM_REGISTER; i++)
		if (ktt_status_past[i] != ktt_status_now[i])
			return true;

	return false;
}


/// @brief 消抖处理后，再次对比键态
bool ktt_is_valid_diff(void) {
	if (ktt_is_diff()) {
		ktt_update_past();
		tool_delay_u_nop(100);	// 抖动
		ktt_read_now();

		if (!ktt_is_diff())
			return true;
	}

	return false;
}


void ktt_set_ktt_hid_wp(void) {
	ktt_hid_pack = (hid_pack_t*)hid_set_wp(
#		if defined(CONFIG_COMM_FORMAT_HID_XXX)
#		elif defined(CONFIG_COMM_FORMAT_HID_CH9329)
		HID_CH9329_CMD_SEND_KB_GENERAL_DATA,
#		endif
		KTT_NUM_REGISTER,
		ktt_status_now,
		ktt_keymap
	);
}


static void ktt_uart_write_hid_wp(void) {
	uart_write(
		UART_BUS_ID(1),
		(const u8*)&hid_wp,
#		if defined(CONFIG_COMM_FORMAT_HID_XXX)
#		elif defined(CONFIG_COMM_FORMAT_HID_CH9329)
		hid_wp.length + 6
#		endif
	);
}


static void ktt_sle_write_hid_wp(void) {
	sle_write(
		tmptest,	// pc,
		(const u8*)&hid_wp,
#		if defined(CONFIG_COMM_FORMAT_HID_XXX)
#		elif defined(CONFIG_COMM_FORMAT_HID_CH9329)
		hid_wp.length + 6
#		endif
	);
}


static void ktt_merge_hid_wp(void) {
	// 以 ktt 为主，不考虑重复键
	if (ktt_hid_pack == NULL) {
		hid_wp = other_hid_pack;
		return;
	}

#	if defined(CONFIG_COMM_FORMAT_HID_XXX)
#	elif defined(CONFIG_COMM_FORMAT_HID_CH9329)
	hid_wp = *ktt_hid_pack;

	u8 i = 2, j = 2;
	for (; i < 8 && ktt_hid_pack->data[i]; i++);
	while (i < 8 && j < 8 && other_hid_pack.data[j])
		hid_wp.data[i++] = (u8)other_hid_pack.data[j++];	// (u8) 省略会出错

	if (other_hid_pack.data[8] > 0)
		hid_wp.data[8] += other_hid_pack.data[8] - 0x0C;
#	endif
}


void ktt_send_hid_wp(void) {
	ktt_merge_hid_wp();

	switch (comm_way) {
	default:					comm_way = COMM_WAY_SLE;
	/****/case COMM_WAY_SLE:	ktt_sle_write_hid_wp();
	break;case COMM_WAY_UART:	ktt_uart_write_hid_wp();
	}
}
