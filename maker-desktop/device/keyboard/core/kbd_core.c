#include "kbd_core.h"
#include "keyboard_def.h"

#include "def.h"
#include "hhid.h"
#include "ssle.h"
#include "tool.h"
#include "uuart.h"

#include <gpio.h>
#include <pinctrl.h>



static u8 kbd_status_past[KBD_NUM_REGISTER] = {};
static u8 kbd_status_now[KBD_NUM_REGISTER] = {};
static const s16 kbd_keymap[KBD_NUM_REGISTER*8] = {
	Tab,     Q,       W,       E,         Lock_Caps, A,         S,        D,
	Esc,     F1,      F2,      F3,        F4,        F5,        F6,       F7,
	BQuote,  Num_1,   Num_2,   Num_3,     Num_7,     Num_6,     Num_5,    Num_4,
	Z,       X,       C,       Ctrl_L,    GUI_L,     Alt_L,     Space,    Shift_L,
	R,       T,       Y,       U,         F,         G,         H,        J,
	Menu,    Fn,      Alt_R,   Comma,     M,         N,         B,        V,
	Minus,   Num_0,   Num_9,   Num_8,     Reserved,  Reserved,  Reserved, Reserved,
	Equal,   BSpace,  BSlash,  Bracket_R, Bracket_L, P,         O,        I,
	PgDn,    End,     Del,     Enter,     Quote,     Semicolon, L,        K,
	Arrow_R, Arrow_D, Arrow_L, Ctrl_R,    Arrow_U,   Shift_R,   Slash,    Period,
	F8,      F9,      F10,     F11,       F12,       Ins,       Home,     PgUp
};


void kbd_init_pin(void) {	// 都初始化为 GPIO模式
	// CE
	uapi_pin_set_mode(KBD_74HC165_PIN_CE, PIN_MODE_0);
	uapi_gpio_set_dir(KBD_74HC165_PIN_CE, GPIO_DIRECTION_OUTPUT);
	uapi_gpio_set_val(KBD_74HC165_PIN_CE, GPIO_LEVEL_LOW);

	// CP (CLK-手动)
	uapi_pin_set_mode(KBD_74HC165_PIN_CP, PIN_MODE_0);
	uapi_gpio_set_dir(KBD_74HC165_PIN_CP, GPIO_DIRECTION_OUTPUT);
	uapi_gpio_set_val(KBD_74HC165_PIN_CP, GPIO_LEVEL_LOW);

	// PL
	uapi_pin_set_mode(KBD_74HC165_PIN_PL, PIN_MODE_0);
	uapi_gpio_set_dir(KBD_74HC165_PIN_PL, GPIO_DIRECTION_OUTPUT);
	uapi_gpio_set_val(KBD_74HC165_PIN_PL, GPIO_LEVEL_HIGH);

	// Q7 (MISO)
	uapi_pin_set_mode(KBD_74HC165_PIN_Q7, PIN_MODE_0);
	uapi_gpio_set_dir(KBD_74HC165_PIN_Q7, GPIO_DIRECTION_INPUT);
}


void kbd_update_past(void) {
	// 键态[现] --> 键态[过]
	for (u8 i=0; i<KBD_NUM_REGISTER; i++)
		kbd_status_past[i] = kbd_status_now[i];
}


/// @attention	若非特殊标注，别处的01bit都指01bit(0up, 1down)
void kbd_read_now(void) {		// 寄存器：74HC165
	static u8 byte = 0;

	// 电平 ----转为---> 01bit(0down,1up) ----并行存入---> 寄存器
	tool_gpio_refresh(KBD_74HC165_PIN_PL, 1);

	// 01bit(0down,1up) ----01反转---> 01bit(0up,1down) ----串行存入---> keyboard_now
	for (u8 i=0; i<KBD_NUM_REGISTER; i++) {		// 每完整扫描，逐寄存器读取
		for (u8 j=0; j<8; j++) {			// 每寄存器，逐bit读取
			byte |= !uapi_gpio_get_val(KBD_74HC165_PIN_Q7) << j;	// 01反转
			tool_gpio_refresh(KBD_74HC165_PIN_CP, 1);
		}
		kbd_status_now[i] = byte;
		byte = 0;
	}
}


/// @brief 不处理，直接对比键态
static bool kbd_is_diff(void) {
	for (u8 i=0; i<KBD_NUM_REGISTER; i++)
		if (kbd_status_past[i] != kbd_status_now[i])
			return true;

	return false;
}


/// @brief 消抖处理后，再次对比键态
bool kbd_is_valid_diff(void) {
	if (kbd_is_diff()) {
		// tool_sleep_m(5);	// 抖动时间(多线程)
		tool_delay_u(100);	// 抖动时间(单线程)
		kbd_read_now();
		kbd_update_past();
		if (!kbd_is_diff())
			return true;
	}

	return false;
}


void kbd_set_hid_wp(void) {
	hid_set_wp(
		HID_CH9329_CMD_SEND_KB_GENERAL_DATA,
		KBD_NUM_REGISTER,
		kbd_status_now,
		kbd_keymap
	);
}


static void kbd_uart_write_hid_wp(void) {
	uart_write(
		1,
		(const u8*)hid_get_wp(),
		hid_get_wp()->length + 6
	);
}


static void kbd_sle_write_hid_wp(void) {
	sle_write(
		0,
		(const u8*)hid_get_wp(),
		hid_get_wp()->length + 6
	);
}


void kbd_send_hid_wp(void) {
	// kbd_uart_write_hid_wp();
	kbd_sle_write_hid_wp();
}
