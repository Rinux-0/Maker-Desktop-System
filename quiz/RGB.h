#ifndef _RGB_H_
#define _RGB_H_

#include "my_def.h"
#include "my_tool.h"
#include "my_tool_spi.h"
#include "main.h"

#define SPIRGB_HIGH 0xF8
#define SPIRGB_LOW 0xC0

#define WS2812_NUM 5
#define WS2812_BYTES      (WS2812_NUM * 3)

#define WS2812_RED       {0x00,0xFF,0x00}
#define WS2812_GREEN     {0xFF,0x00,0x00}
#define WS2812_BLUE      {0x00,0x00,0xFF}
#define WS2812_WHITE     {0xFF,0xFF,0xFF}
#define WS2812_BLACK     {0x00,0x00,0x00}

void RGB_byte_to_spi(uint8_t val, uint8_t* out);
void RGB_to_spi(const uint8_t grb[][3], uint8_t spi_out[]);
void RGB_show(const uint8_t rgb[WS2812_NUM][3]);
void RGB_fill(uint8_t r, uint8_t g, uint8_t b);
void RGB_rainbow(uint8_t shift);
void RGB_wave(uint8_t step);
void RGB_clear(uint8_t num);

#endif
