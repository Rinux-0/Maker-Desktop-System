#pragma once
#ifndef _KBD_OLED_H_
#define _KBD_OLED_H_



#include "ddef.h"

#define BYTES_PER_FRAME   976      // pro1~5 实际长度；如为 1024 请相应改动
#define FRAME_DELAY_MS    100      // 帧间延时，决定帧率（≈10 fps）

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



#endif // _KBD_OLED_H_
