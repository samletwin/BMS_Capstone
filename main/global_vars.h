#ifndef GLOBAL_VARS_H
#define GLOBAL_VARS_H

#include <stdint.h>
#include <stdbool.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t socPerc_ui8;
    uint8_t sohPerc_ui8;
    float batRes_f32;
    float batOcv_f32;
    uint32_t batTts_ui32;
    uint16_t adcBatVolt_mV_ui16;
    uint16_t adcBatCurrent_mA_ui16;
} ui_displayDataType;

typedef struct {
    uint16_t sampleRate_hz_ui16;
    uint16_t dischargePeriod_ms_ui16;
    uint8_t numDischarges_ui8;
} soh_configDataType;

typedef enum {
    UI_SWITCH_OFF = 0,
    UI_SWITCH_ON = 1
} ui_switchStatusType;

typedef enum {
    BATTERY_STATE_NO_CURRENT = 0,
    BATTERY_STATE_DISCHARGING = 1,
    BATTERY_STATE_CHARGING = 2
} battery_stateType;

typedef enum {
    UI_NO_MEASUREMENT = 0,
    UI_START_MEASUREMENT = 1,
    UI_MEASUREMENT_IN_PROGRESS = 2
} ui_startMeasurementType;

typedef struct {
    ui_startMeasurementType sohMeasurementStatus_e;
    ui_switchStatusType dischargeBattSwitch_e;
} ui_eventDataType;

/* ------------------------------------------------------------------------------------
    SETTER FUNCTIONS
------------------------------------------------------------------------------------- */
/* ADC Config Data */
void set_sohConfig_sampleRate_hz_ui16(uint16_t val);
void set_sohConfig_dischargePeriod_ms_ui16(uint16_t val);
void set_sohConfig_numDischarges_ui8(uint8_t val);

/* UI Event Data */
void set_UI_sohMeasurementStatus_e(ui_startMeasurementType val);
void set_UI_dischargeBattSwitch_e(ui_switchStatusType val);

/* Display Data */
void set_display_adcBatCurrent_mA_ui16(uint16_t val);
void set_display_adcBatVolt_mV_ui16(uint16_t val);
void set_display_batTts_ui32(uint32_t val);
void set_display_batOcv_f32(float val);
void set_display_batRes_f32(float val);
void set_display_sohPerc_ui8(uint8_t val);
void set_display_socPerc_ui8(uint8_t val);

/* ------------------------------------------------------------------------------------
    GETTER FUNCTIONS
------------------------------------------------------------------------------------- */
/* ADC Config Data */
const soh_configDataType* get_all_sohConfigData_ps(void);
uint16_t get_sohConfig_sampleRate_hz_ui16(void);
uint16_t get_sohConfig_dischargePeriod_ms_ui16(void);
uint8_t get_sohConfig_numDischarges_ui8(void);

/* UI Event Data */
const ui_eventDataType* get_all_UI_eventData_ps(void);
ui_startMeasurementType get_UI_sohMeasurementStatus_e(void);
ui_switchStatusType get_UI_dischargeBattSwitch_e(void);

/* Display Data */
const ui_displayDataType* get_all_UI_displayData_ps(void);
uint16_t get_display_adcBatCurrent_mA_ui16(void);
uint16_t get_display_adcBatVolt_mV_ui16(void);
uint32_t get_display_batTts_ui32(void);
float get_display_batOcv_f32(void);
float get_display_batRes_f32(void);
uint8_t get_display_sohPerc_ui8(void);
uint8_t get_display_socPerc_ui8(void);

#ifdef __cplusplus
}
#endif 

#endif /* GLOBAL_VARS_H */