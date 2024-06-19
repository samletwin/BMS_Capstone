#ifndef ADC_H
#define ADC_H

#include "driver/adc.h"

// Define the ADC channels to be used
#define ADC_BATT_VOLTAGE_PIN ADC1_CHANNEL_3
#define ADC_BATT_CURRENT_PIN ADC1_CHANNEL_2

#define R1 9970
#define R2 19890
#define ADC_BATT_VOLTAGE_SCALE (R1+R2)/R2

#define ADC_CURRENT_SCALE (1000/400) /* 400mV/A scaling - divide by 400 to get A, multiply by 1000 to get mA*/
#define ADC_CURRENT_VREF_OFFSET_MV 2500

#ifdef __cplusplus
extern "C" {
#endif

// Function prototypes
void adc_init(void);
uint16_t adc_readBattVoltage_mV_ui16(bool debug);
int16_t adc_readBattCurrent_mA_si16(bool debug);

#ifdef __cplusplus
}
#endif

#endif // ADC_H
