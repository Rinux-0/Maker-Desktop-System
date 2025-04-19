#ifndef MYI2C_H
#define MYI2C_H



#include "def.h"

void SDA_Input_Mode(void);
void SDA_Output_Mode(void);
void SDA_Output(uint16_t val);
void SCL_Output(uint16_t val);
u8 SDA_Input(void);
void I2CStart(void);
void I2CStop(void);
unsigned char I2CWaitAck(void);
void I2CSendAck(void);
void I2CSendNotAck(void);
void I2CSendByte(unsigned char cSendByte);
unsigned char I2CReceiveByte(void);
void I2CInit(void);




#endif
