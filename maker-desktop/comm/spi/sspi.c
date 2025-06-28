#include "sspi.h"
#include "sspi_def.h"

// #include "ddef.h"
#include "ttool.h"

#include <pinctrl.h>
#include <spi.h>



bool spi_bus_is_inited[2];
static spi_xfer_data_t spi_xfer_data[2];
static spi_xfer_data_t spi_xfer_data_empty = {
	.tx_buff = NULL,
	.tx_bytes = 300	/// @todo 临时设置
};



u8 SPI_BUS_ID(u8 bus_id) {
	switch (bus_id) {
	default:		return SPI_BUS_NONE;
	break;case 0:	return 0;
	break;case 1:	return 1;
	}
}
static u8 SPI_OUT_PIN(u8 bus_id) {
	switch (bus_id) {
	default:		return PIN_NONE;
	break;case 0:	return SPI_0_OUT_PIN;
	break;case 1:	return SPI_1_OUT_PIN;
	}
}



static void spi_init_pin(u8 bus_id) {
	pin_t pin_out = SPI_OUT_PIN(bus_id);

	if (bus_id == 0) {
		uapi_pin_set_mode(pin_out, 6);
	} else {
		uapi_pin_set_mode(pin_out, 3);
	}

	uapi_pin_set_ds(pin_out, PIN_DS_MAX);
}


static void spi_init_cfg(u8 bus_id) {
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

	uapi_spi_init(bus_id, &config, &ext_config);
}


void spi_init(u8 bus_id) {
	// DATA("\t%d\t%d\n", bus_id, spi_bus_is_inited[bus_id]);

	if (bus_id >= 2 || spi_bus_is_inited[bus_id])
		return;

	spi_init_pin(bus_id);
	spi_init_cfg(bus_id);

	if (!spi_bus_is_inited[0] && !spi_bus_is_inited[1]) {
		spi_xfer_data_empty.tx_buff = (u8*)malloc(spi_xfer_data_empty.tx_bytes * sizeof(u8));
		memset(spi_xfer_data_empty.tx_buff, SPI_FALSE, spi_xfer_data_empty.tx_bytes * sizeof(u8));
	}

	spi_bus_is_inited[bus_id] = true;

	// LOG("");
}


void spi_oneloop(u8 bus_id) {
	if (bus_id >= 2 || !spi_bus_is_inited[bus_id])
		return;
}


void spi_exit(u8 bus_id) {
	if (bus_id >= 2 || !spi_bus_is_inited[bus_id])
		return;

	free(spi_xfer_data_empty.tx_buff);
}


void spi_write(u8 bus_id, const u8* data, u32 length) {
	// DATA("\t%d\t%d\n", bus_id, spi_bus_is_inited[bus_id]);

	if (bus_id >= 2 || !spi_bus_is_inited[bus_id])
		return;

	if (data != NULL && length != 0) {
		spi_xfer_data[bus_id].tx_buff = (u8*)data;
		spi_xfer_data[bus_id].tx_bytes = length;
	}

	uapi_spi_master_write(bus_id, &spi_xfer_data[bus_id], 0xFFFFFFFF);

	// LOG("");
}


void spi_write_empty(u8 bus_id) {
	if (bus_id >= 2 || !spi_bus_is_inited[bus_id])
		return;

	uapi_spi_master_write(bus_id, &spi_xfer_data_empty, 0xFFFFFFFF);
}
