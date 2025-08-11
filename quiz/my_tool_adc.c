#include "my_tool_adc.h"

adc_scan_config_t config = {
		.type = 0,
		.freq = 1,
};

void myadc_callback(uint8_t ch, uint32_t* buffer, uint32_t length, bool* next) {
	UNUSED(next);

	//无滤波
	// char tx[30]=" ";
	// float data = buffer[length - 1];
	// sprintf(tx,"adc:%.2f",data);
	// OLED_ShowString(1,1,tx);

	//平均值滤波
	char tx[30] = " ";
	float result = 0;
	static uint8_t num;
	static float data[ADCDATANUM] = { 0 };
	if (num < 100) {
		data[num++] = buffer[length - 1] / 1000.0;
	} else {
		for (int i = 0;i < ADCDATANUM - 1;i++) {
			data[i] = data[i + 1];
		}
		data[ADCDATANUM - 1] = buffer[length - 1] / 1000.0;

		float sum = 0;
		for (int i = 0;i < ADCDATANUM;i++) {
			sum += data[i];
		}result = 1.0 * sum / ADCDATANUM;
		sprintf(tx, "adc:%f", result);
		OLED_ShowString(1, 1, tx);
	}
}

void myadc_init(void) {
	uapi_adc_init(ADC_CLOCK_500KHZ);
	uapi_adc_power_en(AFE_SCAN_MODE_MAX_NUM, true);
}

//gpio 7——adc 0
//gpio 8——adc 1
//gpio 9——adc 2
//gpio 10——adc 3
//gpio 11——adc 4
//gpio 12——adc 5
void myadc_loop(pin_t pin) {
	uapi_adc_auto_scan_ch_enable(pin - 7, config, myadc_callback);
	uapi_adc_auto_scan_ch_disable(pin - 7);
}
