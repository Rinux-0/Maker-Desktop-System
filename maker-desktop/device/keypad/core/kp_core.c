#include "kp_core.h"

#include "../keypad_def.h"
#include "comm_hid.h"
#include "util_tool.h"

#include <gpio.h>
#include <pinctrl.h>



static u8 kp_status_past[NUM_REGISTER] = {};
static u8 kp_status_now[NUM_REGISTER] = {};
static const s16 kp_keymap[NUM_KEY] = {
	A, B, C, D,
	E, F, G, H,
	I, J, K, L,
	M, N, O, P
};


void kp_pin_init(void) {	// 都初始化为 GPIO模式
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


void kp_past_update(void) {
	// 键态[现] --> 键态[过]
	for (u8 i=0; i<NUM_REGISTER; i++)
		kp_status_past[i] = kp_status_now[i];
}


/// @attention	若非特殊标注，别处的01bit都指01bit(0up, 1down)
void kp_now_read(void) {
	static u8 byte = 0;

	// 电平 ----转为---> 01bit(0down,1up) ----并行存入---> 寄存器
	gpio_refresh(HC165_PL_PIN, 1);

	// 01bit(0down,1up) ----01反转---> 01bit(0up,1down) ----串行存入---> keypad_now
	for (u8 i=0; i<NUM_REGISTER; i++) {		// 每完整扫描，逐寄存器读取
		for (u8 j=0; j<8; j++) {			// 每寄存器，逐bit读取
			byte |= !uapi_gpio_get_val(HC165_Q7_PIN) << j;	// 01反转
			gpio_refresh(HC165_CP_PIN, 1);
		}
		kp_status_now[i] = byte;
		byte = 0;
	}
}


/// @brief 不处理，直接对比键态
/// @return 是否有 键态变化（直接对比）
static bool kp_is_diff(void) {
	for (u8 i=0; i<NUM_REGISTER; i++)
		if (kp_status_past[i] != kp_status_now[i])
			return true;

	return false;
}


/// @brief 消抖处理后，再次对比键态
/// @return 是否有 键态变化
bool kp_is_valid_diff(void) {
	if (kp_is_diff()) {
		udelay(60);
		kp_now_read();
		kp_past_update();
		if (!kp_is_diff())
			return true;
	}

	return false;
}


void kp_hid_wp_construct(void) {
	hid_wp_construct(
		CMD_SEND_KB_GENERAL_DATA,
		NUM_REGISTER,
		kp_status_now,
		kp_keymap
	);
}
