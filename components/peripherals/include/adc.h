#ifndef ADC_READ_H
#define ADC_READ_H

#include "driver/adc.h"

// Define the ADC channels to be used
#define ADC_BATT_VOLTAGE_PIN ADC1_CHANNEL_3
#define ADC_BATT_CURRENT_PIN ADC1_CHANNEL_2

#define R1 9970
#define R2 19890
#define ADC_BATT_VOLTAGE_SCALE (R1+R2)/R2
#define ADC_CURRENT_SCALE (1000/400) /* 400mV/A scaling - divide by 400 to get A, multiply by 1000 to get mA*/

// Function prototypes
void adc_init(void);
uint16_t adc_readBattVoltage_mV(bool debug);
uint16_t adc_readBattCurrent_mA(bool debug);



#endif // ADC_READ_H
