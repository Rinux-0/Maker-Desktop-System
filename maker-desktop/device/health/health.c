#include "health.h"
#include "health_def.h"

#include "ddef.h"
#include "ttool.h"

#if defined(HEALTH_FINGERPRINT)
#	include "fingerprint/fingerprint.h"
#endif
#if defined(HEALTH_DISTANCE)
#	include "distance/distance.h"
#endif
#if defined(HEALTH_HEART)
#	include "heart/heart.h"
#endif
#if defined(HEALTH_TEMPRETURE)
#	include "tempreture/tempreture.h"
#endif



/* 多选 */
void health_uart_r_int_handler(const void* buffer, u16 length, bool error) {
	/*a*/fingerprint_uart_r_int_handler(buffer, length, error);
	/*b1*/heart_uart_r_int_handler(buffer, length, error);
	/*b2*/tempreture_uart_r_int_handler(buffer, length, error);
}


/* 多选 */
void health_init(void) {
	/*a*/fingerprint_init();
	/*b1*/distance_init();
	/*b2*/heart_init();
	/*b3*/tempreture_init();
}


/* 多选 */
void health_oneloop(void) {
	/*a*/fingerprint_oneloop();
	/*b1*/distance_oneloop();
	/*b2*/heart_oneloop();
	/*b3*/tempreture_oneloop();
}


/* 多选 */
void health_exit(void) {
	/*a*/fingerprint_exit();
	/*b1*/distance_exit();
	/*b2*/heart_exit();
	/*b3*/tempreture_exit();
}
