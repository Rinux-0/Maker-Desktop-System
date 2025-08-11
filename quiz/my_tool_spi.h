#ifndef _MY_TOOL_SPI_H_
#define _MY_TOOL_SPI_H_

#include  "my_def.h"
#include  "my_tool.h"
#include <pinctrl.h>
#include <spi.h>
#include  "main.h"

extern spi_xfer_data_t myspi_data, spi_rgb_empty;

void myspi_init(void);
void myspi_write(const uint8_t* data, uint32_t length);
void myspi_write_empty(uint8_t num);

#endif
