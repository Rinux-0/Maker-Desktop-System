#ifndef _SSPI_H_
#define _SSPI_H_



#include "ddef.h"
#include "sspi_def.h"

void spi_init(void);
void spi_oneloop(void);
void spi_exit(void);

void spi_write(const u8* data, u32 length);



#endif // _SSPI_H_
