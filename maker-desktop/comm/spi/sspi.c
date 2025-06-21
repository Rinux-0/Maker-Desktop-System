#include "sspi.h"

#include "ddef.h"
#include "ttool.h"

#include "sspi_def.h"
#include <spi.h>
// #include <stdarg.h>
// #include <securec.h>



static spi_xfer_data_t spi_xfer_data;
static spi_xfer_data_t spi_xfer_data_empty = {
	.tx_buff = NULL,
	.tx_bytes = 300
};



static void spi_init_pin(void) {
	uapi_pin_set_mode(SPI_DO_PIN, 6);
	uapi_pin_set_mode(SPI_CLK_PIN, 3);
	uapi_pin_set_mode(SPI_DI_PIN, 3);

	uapi_pin_set_ds(SPI_DO_PIN, PIN_DS_MAX);
}


static void spi_init_cfg(void) {
	spi_attr_t config = {
		.is_slave = false,
		.slave_num = SPI_SLAVE_NUM,
		.bus_clk = 32000000,
		.freq_mhz = SPI_FREQUENCY,
		.clk_polarity = SPI_CLK_POLARITY,
		.clk_phase = SPI_CLK_PHASE,
		.frame_format = SPI_FRAME_FORMAT,
		.spi_frame_format = HAL_SPI_FRAME_FORMAT_STANDARD,
		.frame_size = HAL_SPI_FRAME_SIZE_8,
		.tmod = SPI_TMOD,
		.sste = 1
	};
	spi_extra_attr_t ext_config = {
		.qspi_param.wait_cycles = SPI_WAIT_CYCLES
	};

	uapi_spi_init(SPI_ID, &config, &ext_config);
}


void spi_init(void) {
	spi_init_pin();
	spi_init_cfg();

	spi_xfer_data_empty.tx_buff =
		(u8*)malloc(spi_xfer_data_empty.tx_bytes * sizeof(u8));

	memset(spi_xfer_data_empty.tx_buff, SPI_FALSE, spi_xfer_data_empty.tx_bytes * sizeof(u8));
}


void spi_oneloop(void) {}


void spi_exit(void) {
	free(spi_xfer_data_empty.tx_buff);
}


void spi_write(const u8* data, u32 length) {
	if (data == NULL || length == 0) {
		uapi_spi_master_write(SPI_ID, &spi_xfer_data_empty, 0xFFFFFFFF);
	} else {
		spi_xfer_data.tx_buff = (u8*)data;
		spi_xfer_data.tx_bytes = length;
		uapi_spi_master_write(SPI_ID, &spi_xfer_data, 0xFFFFFFFF);
	}
}
