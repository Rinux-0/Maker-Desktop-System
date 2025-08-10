#include  "my_def.h"
#include  "my_tool.h"

#include <tcxo.h>



void my_tool_delay_u_nop(u16 time_us) {
	for (u32 i = 0; i < time_us * 19; i++)
		__asm("nop");
}
void my_tool_delay_u(u16 time_us) {
	uapi_tcxo_delay_us(time_us);
}
void my_tool_delay_m(u16 time_ms) {
	uapi_tcxo_delay_ms(time_ms);
}
void my_tool_sleep_m(u16 time_ms) {
	osal_msleep(time_ms);
}



c8* strnstr(const c8* s1, const c8* s2, size_t max_size) {
	if (*s2 == '\0')
		return (c8*)s1;

	for (u8 len = strlen(s2); len <= max_size; max_size--, s1++) {
		if (*s1 == *s2) {
			for (u8 i = 1; ; i++) {
				if (i == len)
					return (c8*)s1;
				if (s1[i] != s2[i])
					break;
			}
		}
	}

	return NULL;
}
