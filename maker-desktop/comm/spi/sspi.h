#ifndef _SSPI_H_
#define _SSPI_H_



#include "ddef.h"
#include "sspi_def.h"

// bus
extern bool spi_bus_is_inited[2];

u8 SPI_BUS_ID(u8 bus_id);

void spi_init(u8 bus_id);
void spi_oneloop(u8 bus_id);
void spi_exit(u8 bus_id);

void spi_write(u8 bus_id, const u8* data, u32 length);
void spi_write_empty(u8 bus_id);



#endif // _SSPI_H_
