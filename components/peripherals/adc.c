#include "adc.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "esp_log.h"

#define TAG "ADC"

// ADC calibration characteristics
static esp_adc_cal_characteristics_t *adc_chars;
static bool adc_is_init = false;

// Function to initialize the ADC
void adc_init(void) {
    if (adc_is_init) 
        return;
    else
        adc_is_init = true;
    // Configure ADC width and attenuation
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC_BATT_VOLTAGE_PIN, ADC_ATTEN_DB_12);
    adc1_config_channel_atten(ADC_BATT_CURRENT_PIN, ADC_ATTEN_DB_12);

    // Characterize ADC at particular atten
    adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_12, ADC_WIDTH_BIT_12, ESP_ADC_CAL_VAL_DEFAULT_VREF, adc_chars);

    ESP_LOGI(TAG, "ADC initialized");
}

// Function to read battery voltage
uint16_t adc_readBattVoltage_mV(bool debug) {
    if (!adc_is_init)
        ESP_LOGE(TAG, "Attempting to read ADC when it is not init!");
    uint32_t adc_reading = adc1_get_raw(ADC_BATT_VOLTAGE_PIN);
    uint32_t voltage = esp_adc_cal_raw_to_voltage(adc_reading, adc_chars);
    if (true == debug)
        ESP_LOGD(TAG, "Batt Voltage Reading - Raw: %d\tVoltage: %dmV", adc_reading, voltage);
    voltage = voltage * ADC_BATT_VOLTAGE_SCALE;
    return (uint16_t)voltage;
}

// Function to read battery current
uint16_t adc_readBattCurrent_mA(bool debug) {
    if (!adc_is_init)
        ESP_LOGE(TAG, "Attempting to read ADC when it is not init!");
    uint32_t adc_reading = adc1_get_raw(ADC_BATT_CURRENT_PIN);
    uint32_t voltage = esp_adc_cal_raw_to_voltage(adc_reading, adc_chars);
    if (true == debug) 
        ESP_LOGD(TAG, "Batt Current Reading - Raw: %dVoltage: %dmV", adc_reading, voltage);
    voltage = voltage * ADC_CURRENT_SCALE;
    return (uint16_t)voltage;
}
