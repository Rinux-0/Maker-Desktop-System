#include "RGB.h"

uint8_t RGB_RED[3] = { 0x00, 0xFF, 0x00 };
uint8_t RGB_GREEN[3] = { 0xFF,0x00,0x00 };
uint8_t RGB_BLUE[3] = { 0x00,0x00,0xFF };

/* 内部：把单个字节转成 8 个 SPI 字节（0/1 编码） */
void RGB_byte_to_spi(uint8_t val, uint8_t* out) {
	for (uint8_t i = 0; i < 8; ++i) {
		out[i] = (val & 0x80) ? SPIRGB_HIGH : SPIRGB_LOW;
		val <<= 1;
	}
}

/* 内部：把 GRB 数组转成 SPI 帧 */
void RGB_to_spi(const uint8_t grb[][3], uint8_t spi_out[]) {
	for (uint8_t led = 0; led < WS2812_NUM; ++led) {
		/* WS2812 数据顺序：G-R-B，高位先发 */
		RGB_byte_to_spi(grb[led][1], spi_out + led * 24);      // G
		RGB_byte_to_spi(grb[led][0], spi_out + led * 24 + 8);  // R
		RGB_byte_to_spi(grb[led][2], spi_out + led * 24 + 16); // B
	}
}

void RGB_to_spi_num(const uint8_t grb[][3], uint8_t spi_out[]) {
	for (uint8_t led = 0; led < RGB_test_num; ++led) {
		/* WS2812 数据顺序：G-R-B，高位先发 */
		RGB_byte_to_spi(grb[led][1], spi_out + led * 24);      // G
		RGB_byte_to_spi(grb[led][0], spi_out + led * 24 + 8);  // R
		RGB_byte_to_spi(grb[led][2], spi_out + led * 24 + 16); // B
	}
}


void RGB_to_spi_single(const uint8_t grb[3], uint8_t spi_out[]) {
	RGB_byte_to_spi(grb[1], spi_out);      // G
	RGB_byte_to_spi(grb[0], spi_out + 8);  // R
	RGB_byte_to_spi(grb[2], spi_out + 16); // B
}

/* 显示指定颜色数组 */
void RGB_show(const uint8_t rgb[WS2812_NUM][3]) {
	uint8_t frame[WS2812_NUM * 24];
	RGB_to_spi(rgb, frame);
	myspi_write(frame, sizeof(frame));
	/* 发送足够 RESET 低电平：>50us，这里额外发 24 字节 0x00 即可 */
	myspi_write_empty(1);
}

void RGB_show_num(const uint8_t rgb[RGB_test_num][3]) {
	uint8_t frame[RGB_test_num * 24];
	RGB_to_spi_num(rgb, frame);
	myspi_write(frame, sizeof(frame));
	/* 发送足够 RESET 低电平：>50us，这里额外发 24 字节 0x00 即可 */
	myspi_write_empty(1);
}

void RGB_singlershow(const uint8_t rgb[3]) {
	uint8_t frame[1 * 24];
	RGB_to_spi_single(rgb, frame);
	myspi_write(frame, sizeof(frame));
	myspi_write_empty(1);
}

/* 全屏纯色 */
void RGB_fill(uint8_t r, uint8_t g, uint8_t b) {
	uint8_t buf[WS2812_NUM][3];
	for (uint8_t i = 0; i < WS2812_NUM; ++i) {
		buf[i][0] = r;
		buf[i][1] = g;
		buf[i][2] = b;
	}
	RGB_show(buf);
}

/* 彩虹环：shift 0~255 改变色相 */
void RGB_rainbow(uint8_t shift) {
	uint8_t buf[WS2812_NUM][3];
	for (uint8_t i = 0; i < WS2812_NUM; ++i) {
		uint8_t pos = (shift + i * 51) & 0xFF;   // 5 颗灯
		/* 简易 HSV->RGB，只取色相 */
		if (pos < 85) {
			buf[i][0] = 0;           // R
			buf[i][1] = pos * 3;     // G
			buf[i][2] = 255 - pos * 3; // B
		} else if (pos < 170) {
			pos -= 85;
			buf[i][0] = pos * 3;
			buf[i][1] = 255 - pos * 3;
			buf[i][2] = 0;
		} else {
			pos -= 170;
			buf[i][0] = 255 - pos * 3;
			buf[i][1] = 0;
			buf[i][2] = pos * 3;
		}
	}
	RGB_show(buf);
}

/* 流水灯：step 0~4 表示当前亮灯位置 */
void RGB_wave(uint8_t step) {
	uint8_t buf[WS2812_NUM][3] = { 0 };
	buf[step % WS2812_NUM][0] = 0xFF;   // 红色
	buf[step % WS2812_NUM][1] = 0x00;
	buf[step % WS2812_NUM][2] = 0x00;
	RGB_show(buf);
}

void RGB_clear(uint8_t num) {
	myspi_write_empty(num);
}

void RGB_test1(uint8_t shift) {
	uint8_t buf[RGB_test_num][3];
	for (uint8_t i = 1; i < RGB_test_num; ++i) {
		uint8_t pos = (shift + i * 31) & 0xFF;   // 8 颗灯
		/* 简易 HSV->RGB，只取色相 */
		if (pos < 85) {
			buf[i][0] = 0;           // R
			buf[i][1] = pos * 3;     // G
			buf[i][2] = 255 - pos * 3; // B
		} else if (pos < 170) {
			pos -= 85;
			buf[i][0] = pos * 3;
			buf[i][1] = 255 - pos * 3;
			buf[i][2] = 0;
		} else {
			pos -= 170;
			buf[i][0] = 255 - pos * 3;
			buf[i][1] = 0;
			buf[i][2] = pos * 3;
		}
	}
	buf[0][0] = 0xFF;
	buf[0][1] = 0x00;
	buf[0][2] = 0x00;

	RGB_show_num(buf);
}


void RGB_test2(uint8_t duty) {
	uint8_t buf[RGB_test_num][3];
	for (uint8_t i = 1; i < RGB_test_num; ++i) {
		buf[i][0] = duty;
		buf[i][1] = duty;
		buf[i][2] = duty;
	}

	buf[0][0] = 0xFF;
	buf[0][1] = 0x00;
	buf[0][2] = 0x00;

	RGB_show_num(buf);
}

void RGB_test3(float adcdata) {
	RGB_test2(adcdata);
}
