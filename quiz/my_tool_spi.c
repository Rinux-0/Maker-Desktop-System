#include "my_tool_spi.h"

spi_xfer_data_t myspi_data;
spi_xfer_data_t spi_rgb_empty = {
	.tx_buff = NULL,
	.tx_bytes = 24
};

void myspi_init(void) {
	spi_attr_t config = {
		.is_slave = false,
		.slave_num = 1,
		.bus_clk = 32e6,
		.freq_mhz = 8,
		.clk_polarity = 1,
		.clk_phase = 1,
		.frame_format = 0,
		.spi_frame_format = HAL_SPI_FRAME_FORMAT_STANDARD,
		.frame_size = HAL_SPI_FRAME_SIZE_8,
		.tmod = 0,
		.sste = 1
	};
	spi_extra_attr_t ext_config = {
		.qspi_param.wait_cycles = 0x10
	};

	uapi_pin_set_mode(9, 3);
	uapi_pin_set_mode(6, 6);
	uapi_spi_init(0, &config, &ext_config);
}

void myspi_write(const uint8_t* data, uint32_t length) {

	if (data != NULL && length != 0) {
		myspi_data.tx_buff = (u8*)data;
		myspi_data.tx_bytes = length;
	}

	uapi_spi_master_write(0, &myspi_data, 0xFFFFFFFF);
}


void myspi_write_empty(uint8_t num) {
	while (num--)
		uapi_spi_master_write(0, &spi_rgb_empty, 0xFFFFFFFF);
}
