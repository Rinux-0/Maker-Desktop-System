#include  "my_def.h"
#include  "my_tool.h"

#include <watchdog.h>




void my_dog_init(u8 time_s) {
	uapi_watchdog_deinit();
	uapi_watchdog_init(0);

	uapi_watchdog_enable(WDT_MODE_RESET);

	uapi_watchdog_set_time(time_s);
}


void my_dog_kick(void) {
	uapi_watchdog_kick();
}
