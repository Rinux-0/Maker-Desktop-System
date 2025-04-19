#include "common_def.h"
#include "osal_debug.h"
#include "cmsis_os2.h"
#include "app_init.h"
#include "pinctrl.h"
#include "tcxo.h"
#include "ssd1306_fonts.h"
#include "ssd1306.h"
#include "i2c.h"
#include "gpio.h"
#include <unistd.h>



#define SUCCESS 0
#define ERROR   1

#define SCL_pin 0
#define SDA_pin 2


//
void SDA_Input_Mode(void) {
	uapi_gpio_set_dir(SDA_pin, GPIO_DIRECTION_INPUT);
}


//
void SDA_Output_Mode(void) {
	uapi_gpio_set_dir(SDA_pin, GPIO_DIRECTION_OUTPUT);
}


//
void SDA_Output(uint16_t val) {
	if (val) {
		uapi_gpio_set_val(SDA_pin, 1);
	} else {
		uapi_gpio_set_val(SDA_pin, 0);
	}
}


//
void SCL_Output(uint16_t val) {
	if (val) {
		uapi_gpio_set_val(SCL_pin, 1);
	} else {
		uapi_gpio_set_val(SCL_pin, 0);
	}
}


//
uint8_t SDA_Input(void) {
	if (uapi_gpio_get_val(SDA_pin)== 1) {
		return 1;
	} else {
		return 0;
	}
}


//
void I2CStart(void) {
	SDA_Output(1);
	usleep(5);
	SCL_Output(1);
	usleep(5);
	SDA_Output(0);
	usleep(5);
	SCL_Output(0);
	usleep(5);
}


//
void I2CStop(void) {
	SCL_Output(0);
	usleep(5);
	SDA_Output(0);
	usleep(5);
	SCL_Output(1);
	usleep(5);
	SDA_Output(1);
	usleep(5);

}


//
unsigned char I2CWaitAck(void) {
	unsigned short cErrTime = 5;
	SDA_Input_Mode();
	usleep(5);
	SCL_Output(1);
	usleep(5);
	while (SDA_Input()) {
		cErrTime--;
		usleep(5);
		if (0 == cErrTime) {
			SDA_Output_Mode();
			I2CStop();
			return ERROR;
		}
	}
	SCL_Output(0);
	SDA_Output_Mode();
	usleep(5);
	return SUCCESS;
}


//
void I2CSendAck(void) {
	SDA_Output(0);
	usleep(5);
	usleep(5);
	SCL_Output(1);
	usleep(5);
	SCL_Output(0);
	usleep(5);

}


//
void I2CSendNotAck(void) {
	SDA_Output(1);
	usleep(5);
	usleep(5);
	SCL_Output(1);
	usleep(5);
	SCL_Output(0);
	usleep(5);

}


//
void I2CSendByte(unsigned char cSendByte) {
	unsigned char  i = 8;
	while (i--) {
		SCL_Output(0);
		usleep(5);
		SDA_Output(cSendByte & 0x80);
		usleep(5);
		cSendByte += cSendByte;
		usleep(5);
		SCL_Output(1);
		usleep(5);
	}
	SCL_Output(0);
	usleep(5);
}


//
unsigned char I2CReceiveByte(void) {
	unsigned char i = 8;
	unsigned char cR_Byte = 0;
	SDA_Input_Mode();
	while (i--) {
		cR_Byte += cR_Byte;
		SCL_Output(0);
		usleep(5);
		usleep(5);
		SCL_Output(1);
		usleep(5);
		cR_Byte |=  SDA_Input();
	}
	SCL_Output(0);
	usleep(5);
	SDA_Output_Mode();
	return cR_Byte;
}


//
void I2CInit(void) {
	uapi_pin_set_mode(SDA_pin, 0);
	uapi_pin_set_mode(SCL_pin, 0);
	uapi_gpio_set_dir(SCL_pin, 1);
	uapi_gpio_set_dir(SDA_pin, 1);
	uapi_gpio_set_val(SDA_pin, 1);
	uapi_gpio_set_val(SCL_pin, 1);
	uapi_pin_set_pull(SDA_pin, PIN_PULL_TYPE_UP);
	uapi_pin_set_pull(SCL_pin, PIN_PULL_TYPE_UP);
}
