#ifndef _MYI2C_H_
#define _MYI2C_H_

#include "common_def.h"
#include "osal_debug.h"
#include "cmsis_os2.h"
#include "app_init.h"
#include "pinctrl.h"
#include <unistd.h>
#include "main.h"

#define SUCCESS 0
#define ERRORRR 1

#define SCL_pin 16
#define SDA_pin 15

void SDA_Input_Mode(void);
void SDA_Output_Mode(void);
void SDA_Output(uint16_t val);
void SCL_Output(uint16_t val);
uint8_t SDA_Input(void);
void I2CStart(void);
void I2CStop(void);
unsigned char I2CWaitAck(void);
void I2CSendAck(void);
void I2CSendNotAck(void);
void I2CSendByte(unsigned char cSendByte);
unsigned char I2CReceiveByte(void);
void I2CInit(void);

#endif
