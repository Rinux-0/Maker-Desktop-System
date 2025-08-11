#ifndef __OLED_H
#define __OLED_H
#include "OLED_Font.h"
#include "common_def.h"
#include "osal_debug.h"
#include "cmsis_os2.h"
#include "app_init.h"
#include "pinctrl.h"
#include <unistd.h>
#include "myi2c.h"
#include "stdint.h"

#define OLED_SCL_pin 16
#define OLED_SDA_pin 15


void OLED_Init_1309(void);
void OLED_Init(void);
void OLED_Clear(void);
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char);
void OLED_ShowString(uint8_t Line, uint8_t Column, char* String);
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length);
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_ShowPix(const uint8_t* image);
void OLED_ShowGif(const uint8_t* const frames[], uint8_t frameCnt);
#endif
