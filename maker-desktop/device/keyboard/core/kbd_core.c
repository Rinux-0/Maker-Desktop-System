#include "kbd_core.h"
#include "keyboard_def.h"

#include "ddef.h"
#include "ttool.h"
#include "color.h"

#include "comm.h"
#include "hhid.h"
#include "ssle.h"
#include "uuart.h"

#include <non_os_reboot.h>
#include <gpio.h>
#include <pinctrl.h>



static hid_pack_t hid_wp = { 0 };
static hid_pack_t* kbd_hid_pack = NULL;
static hid_pack_t other_hid_pack = { 0 };

// [0]up [1]down
static u8 kbd_status_past[KBD_NUM_REGISTER] = {};
static u8 kbd_status_now[KBD_NUM_REGISTER] = {};

static const s16 kbd_keymap[KBD_NUM_REGISTER * 8] = {
	//      0        1        2        3          4          5          6         7
	/*  0 */Tab,     Q,       W,       E,         Lock_Caps, A,         S,        D,
	/*  1 */Esc,     F1,      F2,      F3,        F4,        F5,        F6,       F7,
	/*  2 */BQuote,  Num_1,   Num_2,   Num_3,     Num_7,     Num_6,     Num_5,    Num_4,
	/*  3 */Z,       X,       C,       Ctrl_L,    GUI_L,     Alt_L,     Space,    Shift_L,
	/*  4 */R,       T,       Y,       U,         F,         G,         H,        J,
	/*  5 */Menu,    Fn,      Alt_R,   Comma,     M,         N,         B,        V,
	/*  6 */Minus,   Num_0,   Num_9,   Num_8,     Reserved,  Reserved,  Reserved, Reserved,
	/*  7 */Equal,   BSpace,  BSlash,  Bracket_R, Bracket_L, P,         O,        I,
	/*  8 */PgDn,    End,     Del,     Enter,     Quote,     Semicolon, L,        K,
	/*  9 */Arrow_R, Arrow_D, Arrow_L, Ctrl_R,    Arrow_U,   Shift_R,   Slash,    Period,
	/* 10 */F8,      F9,      F10,     F11,       F12,       Ins,       Home,     PgUp
};



void kbd_init_pin(void) {	// 都初始化为 GPIO模式
	// CE
	uapi_pin_set_mode(KBD_PIN_74HC165_CE, PIN_MODE_0);
	uapi_gpio_set_dir(KBD_PIN_74HC165_CE, GPIO_DIRECTION_OUTPUT);
	uapi_gpio_set_val(KBD_PIN_74HC165_CE, GPIO_LEVEL_LOW);

	// CP (CLK-手动)
	uapi_pin_set_mode(KBD_PIN_74HC165_CP, PIN_MODE_0);
	uapi_gpio_set_dir(KBD_PIN_74HC165_CP, GPIO_DIRECTION_OUTPUT);
	uapi_gpio_set_val(KBD_PIN_74HC165_CP, GPIO_LEVEL_LOW);

	// PL
	uapi_pin_set_mode(KBD_PIN_74HC165_PL, PIN_MODE_0);
	uapi_gpio_set_dir(KBD_PIN_74HC165_PL, GPIO_DIRECTION_OUTPUT);
	uapi_gpio_set_val(KBD_PIN_74HC165_PL, GPIO_LEVEL_HIGH);

	// Q7 (MISO)
	uapi_pin_set_mode(KBD_PIN_74HC165_Q7, PIN_MODE_0);
	uapi_gpio_set_dir(KBD_PIN_74HC165_Q7, GPIO_DIRECTION_INPUT);

	// SLE
	uapi_pin_set_mode(LED_PIN_SLE, PIN_MODE_0);
	uapi_gpio_set_dir(LED_PIN_SLE, GPIO_DIRECTION_OUTPUT);
	uapi_gpio_set_val(LED_PIN_SLE, GPIO_LEVEL_HIGH);
}


static void kbd_uart_r_int_handler(const void* buffer, u16 length, bool error) {
	unused(error);

	u8* buff_rx = (u8*)buffer;
	u8* p_other = (u8*)&other_hid_pack;

	// DATA("length=%d\n"
	// 	"\t[h1]%02x [h2]%02x [a]%02x [c]%02x [l]%02x\n"
	// 	"\t[d0]%02x [d1]%02x [d2]%02x [d3]%02x [d4]%02x [d5]%02x [d6]%02x [d7]%02x\n"
	// 	"\t[s]%02x\n",
	// 	length,
	// 	p_other[0], p_other[1], p_other[2], p_other[3], p_other[4],
	// 	p_other[5], p_other[6], p_other[7], p_other[8], p_other[9], p_other[10], p_other[11], p_other[12],
	// 	p_other[13]
	// );

	for (u16 i = 0; i < length; i++) {
		p_other[i] = buff_rx[i];
	}

	kbd_send_hid_wp();
}


void kbd_init_int_cb(void) {
	// 来自rcv (暂时只支持CH9329,无法回传)

	// 来自other
	// LOG("");
	uart_set_r_cb(UART_BUS_ID(2), kbd_uart_r_int_handler);
	LOG("");
}


void kbd_update_past(void) {
	// 键态(现) ---> 键态(过)
	for (u8 i = 0; i < KBD_NUM_REGISTER; i++)
		kbd_status_past[i] = kbd_status_now[i];
}


void kbd_read_now(void) {
	// 并行存入
	tool_pin_gpio_refresh_u(KBD_PIN_74HC165_PL, 1);

	// 串行取出
	for (u8 i = 0; i < KBD_NUM_REGISTER; i++) {		// 逐寄存器
		u8 byte = 0;
		for (u8 j = 0; j < 8; j++) {				// 逐bit
			byte |= !uapi_gpio_get_val(KBD_PIN_74HC165_Q7) << j;	// 01反转
			tool_pin_gpio_refresh_u(KBD_PIN_74HC165_CP, 1);
		}
		kbd_status_now[i] = byte;
	}
}


bool kbd_is_fn_pressed(void) {
	return kbd_status_now[5] & (1 << 1);
}


/// @note 假定 Fn 已按下
void kbd_fn_processer(void) {
	if (kbd_status_now[3] & (1 << 6)) {		// Space	-灯效切换
		color_set_mode_next();
		LOG("");
	} if (kbd_status_now[10] & (1 << 5)) {	// Ins		- color_hsv_h_speed +
		color_ctrl_hsv_h(100);
	} if (kbd_status_now[8] & (1 << 2)) {	// Del		- color_hsv_h_speed -
		color_ctrl_hsv_h(-100);
	} if (kbd_status_now[10] & (1 << 6)) {	// Home		- color_hsv_s_is_changing 切换
		color_ctrl_hsv_s(true, false, 0b00);
	} if (kbd_status_now[8] & (1 << 1)) {	// End		- color_hsv_s_is_full 切换
		color_ctrl_hsv_s(false, true, 0b00);
	} if (kbd_status_now[10] & (1 << 7)) {	// PgUp		- color_hsv_v_is_changing 切换
		color_ctrl_hsv_v(true, false, 0b00);
	} if (kbd_status_now[8] & (1 << 0)) {	// PgDn		- color_hsv_v_is_full 切换
		color_ctrl_hsv_v(false, true, 0b00);
	}

	if (kbd_status_now[0] & (1 << 0)) {	// Tab		-comm_way切换
		if (comm_way++ == COMM_WAY_SLE)
			comm_way = COMM_WAY_UART;
		LOG("");
	}

	if (kbd_status_now[1] & (1 << 0)) {	// Esc		-软复位
		reboot_system(REBOOT_CAUSE_APPLICATION_WATCHDOG);
	}

	LOG("comm_way: %d\ncolor_mode: %d\n", comm_way, color_get_mode());
}


/// @brief 不处理，直接对比键态
static bool kbd_is_diff(void) {
	for (u8 i = 0; i < KBD_NUM_REGISTER; i++)
		if (kbd_status_past[i] != kbd_status_now[i])
			return true;

	return false;
}


/// @brief 消抖处理后，再次对比键态
bool kbd_is_valid_diff(void) {
	if (kbd_is_diff()) {
		kbd_update_past();
		tool_delay_u_nop(100);	// 抖动
		kbd_read_now();

		if (!kbd_is_diff())
			return true;
	}

	return false;
}


void kbd_set_kbd_hid_wp(void) {
	kbd_hid_pack = (hid_pack_t*)hid_set_wp(
#		if defined(CONFIG_COMM_FORMAT_HID_XXX)
#		elif defined(CONFIG_COMM_FORMAT_HID_CH9329)
		HID_CH9329_CMD_SEND_KB_GENERAL_DATA,
#		endif
		KBD_NUM_REGISTER,
		kbd_status_now,
		kbd_keymap
	);
}


static void kbd_uart_write_hid_wp(void) {
	uart_write(
		UART_BUS_ID(1),
		(const u8*)&hid_wp,
#		if defined(CONFIG_COMM_FORMAT_HID_XXX)
#		elif defined(CONFIG_COMM_FORMAT_HID_CH9329)
		hid_wp.length + 6
#		endif
	);
}


static void kbd_sle_write_hid_wp(void) {
	sle_write(
		pc,
		(const u8*)&hid_wp,
#		if defined(CONFIG_COMM_FORMAT_HID_XXX)
#		elif defined(CONFIG_COMM_FORMAT_HID_CH9329)
		hid_wp.length + 6
#		endif
	);
}


static void kbd_merge_hid_wp(void) {
	// 以 kbd 为主，不考虑重复键
	if (kbd_hid_pack == NULL) {
		hid_wp = other_hid_pack;
		return;
	}

#	if defined(CONFIG_COMM_FORMAT_HID_XXX)
#	elif defined(CONFIG_COMM_FORMAT_HID_CH9329)
	hid_wp = *kbd_hid_pack;

	u8 i = 2, j = 2;
	for (; i < 8 && kbd_hid_pack->data[i]; i++);
	while (i < 8 && j < 8 && other_hid_pack.data[j])
		hid_wp.data[i++] = (u8)other_hid_pack.data[j++];	// (u8) 省略会出错

	if (other_hid_pack.data[8] > 0)
		hid_wp.data[8] += other_hid_pack.data[8] - 0x0C;
#	endif
}


void kbd_send_hid_wp(void) {
	kbd_merge_hid_wp();

	switch (comm_way) {
	default:					comm_way = COMM_WAY_SLE;
	/*  */case COMM_WAY_SLE:	kbd_sle_write_hid_wp();
	break;case COMM_WAY_UART:	kbd_uart_write_hid_wp();
	}
}
