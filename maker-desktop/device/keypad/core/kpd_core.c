#include "kpd_core.h"
#include "keypad_def.h"

#include "ddef.h"
#include "hhid.h"
#include "ssle.h"
#include "ttool.h"
#include "uuart.h"

#include <gpio.h>
#include <pinctrl.h>



static u8 kpd_status_past[KPD_NUM_REGISTER] = {};
static u8 kpd_status_now[KPD_NUM_REGISTER] = {};
static const s16 kpd_keymap[KPD_NUM_KEY] = {
	A, B, C, D,
	E, F, G, H,
	I, J, K, L,
	M, N, O, P
};


void kpd_init_pin(void) {	// 都初始化为 GPIO模式
	// CE
	uapi_pin_set_mode(KPD_74HC165_PIN_CE, PIN_MODE_0);
	uapi_gpio_set_dir(KPD_74HC165_PIN_CE, GPIO_DIRECTION_OUTPUT);
	uapi_gpio_set_val(KPD_74HC165_PIN_CE, GPIO_LEVEL_LOW);

	// CP (CLK-手动)
	uapi_pin_set_mode(KPD_74HC165_PIN_CP, PIN_MODE_0);
	uapi_gpio_set_dir(KPD_74HC165_PIN_CP, GPIO_DIRECTION_OUTPUT);
	uapi_gpio_set_val(KPD_74HC165_PIN_CP, GPIO_LEVEL_LOW);

	// PL
	uapi_pin_set_mode(KPD_74HC165_PIN_PL, PIN_MODE_0);
	uapi_gpio_set_dir(KPD_74HC165_PIN_PL, GPIO_DIRECTION_OUTPUT);
	uapi_gpio_set_val(KPD_74HC165_PIN_PL, GPIO_LEVEL_HIGH);

	// Q7 (MISO)
	uapi_pin_set_mode(KPD_74HC165_PIN_Q7, PIN_MODE_0);
	uapi_gpio_set_dir(KPD_74HC165_PIN_Q7, GPIO_DIRECTION_INPUT);
}


void kpd_update_past(void) {
	// 键态[现] --> 键态[过]
	for (u8 i=0; i<KPD_NUM_REGISTER; i++)
		kpd_status_past[i] = kpd_status_now[i];
}


/// @attention	若非特殊标注，别处的01bit都指01bit(0up, 1down)
void kpd_read_now(void) {
	static u8 byte = 0;

	// 电平 ----转为---> 01bit(0down,1up) ----并行存入---> 寄存器
	tool_gpio_refresh(KPD_74HC165_PIN_PL, 1);

	// 01bit(0down,1up) ----01反转---> 01bit(0up,1down) ----串行存入---> keypad_now
	for (u8 i=0; i<KPD_NUM_REGISTER; i++) {		// 每完整扫描，逐寄存器读取
		for (u8 j=0; j<8; j++) {			// 每寄存器，逐bit读取
			byte |= !uapi_gpio_get_val(KPD_74HC165_PIN_Q7) << j;	// 01反转
			tool_gpio_refresh(KPD_74HC165_PIN_CP, 1);
		}
		kpd_status_now[i] = byte;
		byte = 0;
	}
}


/// @brief 不处理，直接对比键态
/// @return 是否有 键态变化（直接对比）
static bool kpd_is_diff(void) {
	for (u8 i=0; i<KPD_NUM_REGISTER; i++)
		if (kpd_status_past[i] != kpd_status_now[i])
			return true;

	return false;
}


/// @brief 消抖处理后，再次对比键态
/// @return 是否有 键态变化
bool kpd_is_valid_diff(void) {
	if (kpd_is_diff()) {
		kpd_update_past();
		tool_delay_u(100);	// 抖动时间(单线程)
		kpd_read_now();

		if (!kpd_is_diff())
			return true;
	}

	return false;
}


void kpd_set_hid_wp(void) {
	hid_set_wp(
		HID_CH9329_CMD_SEND_KB_GENERAL_DATA,
		KPD_NUM_REGISTER,
		kpd_status_now,
		kpd_keymap
	);
}


static void kpd_uart_write_hid_wp(void) {
	uart_write(
		1,
		(const u8*)hid_get_wp(),
		hid_get_wp()->length + 6
	);
}


static void kpd_sle_write_hid_wp(void) {
	sle_write(
		0,
		(const u8*)hid_get_wp(),
		hid_get_wp()->length + 6
	);
}


void kpd_send_hid_wp(void) {
	// kpd_uart_write_hid_wp();
	kpd_sle_write_hid_wp();
}
