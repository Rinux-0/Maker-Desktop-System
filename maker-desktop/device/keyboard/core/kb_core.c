#include "kb_core.h"

#include "../keyboard_def.h"
#include "comm_hid.h"
#include "util_tool.h"

#include <gpio.h>
#include <pinctrl.h>



static u8 kb_status_past[NUM_REGISTER] = {};
static u8 kb_status_now[NUM_REGISTER] = {};
static const s16 kb_keymap[NUM_REGISTER*8] = {
	Tab,     Q,       W,       E,         Lock_Caps, A,         S,        D,
	Esc,     F1,      F2,      F3,        F4,        F5,        F6,       F7,
	BQuote,  Num_1,   Num_2,   Num_3,     Num_7,     Num_6,     Num_5,    Num_4,
	Z,       X,       C,       Ctrl_L,    GUI_L,     Alt_L,     Space,    Shift_L,
	R,       T,       Y,       U,         F,         G,         H,        J,
	Menu,    Fn,      Alt_R,   Comma,     M,         N,         B,        V,
	Minus,   Num_0,   Num_9,   Num_8,     Reserved,  Reserved,  Reserved, Reserved,
	Equal,   BSpace,  BSlash,  Bracket_R, Bracket_L, P,         O,        I,
	Del,     End,     PgDn,    Enter,     Quote,     Semicolon, L,        K,
	Arrow_R, Arrow_D, Arrow_L, Ctrl_R,    Arrow_U,   Shift_R,   Slash,    Period,
	F8,      F9,      F10,     F11,       F12,       Ins,       Home,     PgUp
};


void kb_pin_init(void) {	// 都初始化为 GPIO模式
	// CE
	uapi_pin_set_mode(HC165_CE_PIN, PIN_MODE_0);
	uapi_gpio_set_dir(HC165_CE_PIN, GPIO_DIRECTION_OUTPUT);
	uapi_gpio_set_val(HC165_CE_PIN, GPIO_LEVEL_LOW);

	// CP (CLK-手动)
	uapi_pin_set_mode(HC165_CP_PIN, PIN_MODE_0);
	uapi_gpio_set_dir(HC165_CP_PIN, GPIO_DIRECTION_OUTPUT);
	uapi_gpio_set_val(HC165_CP_PIN, GPIO_LEVEL_LOW);

	// PL
	uapi_pin_set_mode(HC165_PL_PIN, PIN_MODE_0);
	uapi_gpio_set_dir(HC165_PL_PIN, GPIO_DIRECTION_OUTPUT);
	uapi_gpio_set_val(HC165_PL_PIN, GPIO_LEVEL_HIGH);

	// Q7 (MISO)
	uapi_pin_set_mode(HC165_Q7_PIN, PIN_MODE_0);
	uapi_gpio_set_dir(HC165_Q7_PIN, GPIO_DIRECTION_INPUT);
}


void kb_past_update(void) {
	// 键态[现] --> 键态[过]
	for (u8 i=0; i<NUM_REGISTER; i++)
		kb_status_past[i] = kb_status_now[i];
}


/// @attention	若非特殊标注，别处的01bit都指01bit(0up, 1down)
void kb_now_read(void) {		// 寄存器：74HC165
	static u8 byte = 0;

	// 电平 ----转为---> 01bit(0down,1up) ----并行存入---> 寄存器
	gpio_refresh(HC165_PL_PIN, 1);

	// 01bit(0down,1up) ----01反转---> 01bit(0up,1down) ----串行存入---> keyboard_now
	for (u8 i=0; i<NUM_REGISTER; i++) {		// 每完整扫描，逐寄存器读取
		for (u8 j=0; j<8; j++) {			// 每寄存器，逐bit读取
			byte |= !uapi_gpio_get_val(HC165_Q7_PIN) << j;	// 01反转
			gpio_refresh(HC165_CP_PIN, 1);
		}
		kb_status_now[i] = byte;
		byte = 0;
	}
}


/// @brief 不处理，直接对比键态
static bool kb_is_diff(void) {
	for (u8 i=0; i<NUM_REGISTER; i++)
		if (kb_status_past[i] != kb_status_now[i])
			return true;

	return false;
}


/// @brief 消抖处理后，再次对比键态
bool kb_is_valid_diff(void) {
	if (kb_is_diff()) {
		udelay(40);	// 抖动时间
		kb_now_read();
		kb_past_update();
		if (!kb_is_diff())
			return true;
	}

	return false;
}


void kb_hid_wp_construct(void) {
	hid_wp_construct(
		CMD_SEND_KB_GENERAL_DATA,
		NUM_REGISTER,
		kb_status_now,
		kb_keymap
	);
}
