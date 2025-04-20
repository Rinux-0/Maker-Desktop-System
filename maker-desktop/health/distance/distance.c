#include "distance.h"

#include "def.h"
#include "ii2c.h"
#include "tool.h"



u16 distance_get_value(void) {
	u16 data;

	I2CStart();

	I2CSendByte(0xE8);
	I2CWaitAck();
	I2CSendByte(0x10);
	I2CWaitAck();
	I2CSendByte(0xB0);
	I2CWaitAck();

	I2CStop();
	tool_delay_u(40000);
	I2CStart();

	I2CSendByte(0xE8);
	I2CWaitAck();
	I2CSendByte(0x02);
	I2CWaitAck();

	I2CStop();
	I2CStart();

	I2CSendByte(0xE9);
	I2CWaitAck();
	data = I2CReceiveByte();
	I2CSendAck();
	data = data * 256 + I2CReceiveByte();
	I2CSendNotAck();

	I2CStop();

	return data;
}
