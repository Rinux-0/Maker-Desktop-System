#pragma once
#ifndef _SSPI_DEF_H_
#define _SSPI_DEF_H_
// 只考虑输出，不考虑输入
// QSPI 当作 普通SPI， 不分别处理



// SPI_BUS_NONE 为 2

/* SPI_BUS_0 */
// SPI
#define SPI_0_ID			(0)
#define SPI_0_OUT_PIN		(6)
/* SPI_BUS_1 */
// QSPI0
#define SPI_1_ID			(1)
#define SPI_1_OUT_PIN		(1)



// ???
// #define SPI_FRAME_FORMAT_STANDARD 0
// #define SPI_FRAME_SIZE_8 0x1f



#define SPI_TRUE 0xF8
#define SPI_FALSE 0xC0



#endif // _SSPI_DEF_H_
