#include <gpio.h>
#include <pinctrl.h>
#include "keypad.h"

#include "../util/util_tool.h"
#include "keypad_def.h"



static u8 kp_past[NUM_REGISTER] = {};
static u8 kp_now[NUM_REGISTER] = {};
static const u8 keymap[NUM_KEY] = {
	A, B, C, D,
	E, F, G, H,
	I, J, K, L,
	M, N, O, P
};
// static const u8 keymap[NUM_KEY] = {
// 	ESC, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, PAUSE,
// 	GRAVE_ACCENT, NUM_1, NUM_2, NUM_3, NUM_4, NUM_5, NUM_6, NUM_7, NUM_8, NUM_9, NUM_0, MINUS, EQUAL, BACKSPACE,
// 	INSERT,
// 	TAB, Q, W, E, R, T, Y, U, I, O, P, LEFT_U_BRACE, RIGHT_U_BRACE, BACKSLASH, DELETE,
// 	CAP_LOCK, A, S, D, F, G, H, J, K, L, SEMI_COLON, QUOTE, ENTER, PAGE_UP,
// 	LEFT_SHIFT, Z, X, C, V, B, N, M, COMMA, PERIOD, SLASH, RIGHT_SHIFT, UP_ARROW, PAGE_DOWN,
// 	LEFT_CTRL, LEFT_GUI, LEFT_ALT, SPACE, RIGHT_ALT, FN, RIGHT_CTRL, LEFT_ARROW, DOWN_ARROW, RIGHT_ARROW
// };


void keypad_init(void) {	// 都初始化为 GPIO模式
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


void keypad_past_update(void) {
	// 键态[现] --> 键态[过]
	for (u8 i=0; i<NUM_REGISTER; i++)
		kp_past[i] = kp_now[i];
}


/// @attention	若非特殊标注，别处的01bit都指01bit(0up, 1down)
void keypad_now_read(void) {
	static u8 byte = 0;

	// 电平 ----转为---> 01bit(0down,1up) ----并行存入---> 寄存器
	pin_voltage_refresh(HC165_PL_PIN, 1);

	// 01bit(0down,1up) ----01反转---> 01bit(0up,1down) ----串行存入---> keypad_now
	for (u8 i=0; i<NUM_REGISTER; i++) {		// 每完整扫描，逐寄存器读取
		for (u8 j=0; j<8; j++) {			// 每寄存器，逐bit读取
			byte |= !uapi_gpio_get_val(HC165_Q7_PIN) << j;	// 01反转
			pin_voltage_refresh(HC165_CP_PIN, 1);
		}
		kp_now[i] = byte;
		byte = 0;
	}
}


/// @brief 不处理，直接对比键态
/// @return 是否有 键态变化（直接对比）
static bool keypad_is_diff(void) {
	for (u8 i=0; i<NUM_REGISTER; i++)
		if (kp_past[i] != kp_now[i])
			return true;

	return false;
}


/// @brief 消抖处理后，再次对比键态
/// @return 是否有 键态变化
bool keypad_is_valid_diff(void) {
	if (keypad_is_diff()) {
		udelay(80);
		keypad_now_read();
		keypad_past_update();
		if (!keypad_is_diff())
			return true;
	}

	return false;
}


/// @note 暂无get_keypad_past
const u8* get_keypad_now(void) {
	return kp_now;
}


const u8* get_keymap(void) {
	return keymap;
}
