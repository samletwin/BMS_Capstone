#include "global_vars.h"

ui_displayDataType data_s = (ui_displayDataType){0};
soh_configDataType soh_config_data_s = {
    .dischargePeriod_ms_ui16 = 500,
    .numDischarges_ui8 = 2,
    .sampleRate_hz_ui16 = 100
};
ui_eventDataType ui_event_data_s = (ui_eventDataType){0};


/* ------------------------------------------------------------------------------------
    SETTER FUNCTIONS
------------------------------------------------------------------------------------- */
/* ADC Config Data */
void set_sohConfig_sampleRate_hz_ui16(uint16_t val) {
    soh_config_data_s.sampleRate_hz_ui16 = val;
}
void set_sohConfig_dischargePeriod_ms_ui16(uint16_t val) {
    soh_config_data_s.dischargePeriod_ms_ui16 = val;
}
void set_sohConfig_numDischarges_ui8(uint8_t val) {
    soh_config_data_s.numDischarges_ui8 = val;
}

/* UI Event Data */
void set_UI_sohMeasurementStatus_e(ui_startMeasurementType val) {
    ui_event_data_s.sohMeasurementStatus_e = val;
}
void set_UI_dischargeBattSwitch_e(ui_switchStatusType val) {
    ui_event_data_s.dischargeBattSwitch_e = val;
}

/* Display Data */
void set_display_adcBatCurrent_mA_ui16(uint16_t val) {
    data_s.adcBatCurrent_mA_ui16 = val;
}
void set_display_adcBatVolt_mV_ui16(uint16_t val) {
    data_s.adcBatVolt_mV_ui16 = val;
}
void set_display_batTts_ui32(uint32_t val) {
    data_s.batTts_ui32 = val;
}
void set_display_batOcv_f32(float val) {
    data_s.batOcv_f32 = val;
}
void set_display_batRes_f32(float val) {
    data_s.batRes_f32 = val;
}
void set_display_sohPerc_ui8(uint8_t val) {
    data_s.sohPerc_ui8 = val;
}
void set_display_socPerc_ui8(uint8_t val) {
    data_s.socPerc_ui8 = val;
}

/* ------------------------------------------------------------------------------------
    GETTER FUNCTIONS
------------------------------------------------------------------------------------- */
/* ADC Config Data */
const soh_configDataType* get_all_sohConfigData_ps(void) {
    return &soh_config_data_s;
}
uint16_t get_sohConfig_sampleRate_hz_ui16(void) {
    return soh_config_data_s.sampleRate_hz_ui16;
}
uint16_t get_sohConfig_dischargePeriod_ms_ui16(void) {
    return soh_config_data_s.dischargePeriod_ms_ui16;
}
uint8_t get_sohConfig_numDischarges_ui8(void) {
    return soh_config_data_s.numDischarges_ui8; 
}

/* UI Event Data */
const ui_eventDataType* get_all_UI_eventData_ps(void) {
    return &ui_event_data_s;
}
ui_startMeasurementType get_UI_sohMeasurementStatus_e(void) {
    return ui_event_data_s.sohMeasurementStatus_e;
}
ui_switchStatusType get_UI_dischargeBattSwitch_e(void) {
    return ui_event_data_s.dischargeBattSwitch_e;
}

/* Display Data */
const ui_displayDataType* get_all_UI_displayData_ps(void) {
    return &data_s;
}
uint16_t get_display_adcBatCurrent_mA_ui16(void) {
    return data_s.adcBatCurrent_mA_ui16;
}
uint16_t get_display_adcBatVolt_mV_ui16(void) {
    return data_s.adcBatVolt_mV_ui16;
}
uint32_t get_display_batTts_ui32(void) {
    return data_s.batTts_ui32;
}
float get_display_batOcv_f32(void) {
    return data_s.batOcv_f32;
}
float get_display_batRes_f32(void) {
    return data_s.batRes_f32;
}
uint8_t get_display_sohPerc_ui8(void) {
    return data_s.sohPerc_ui8;
}
uint8_t get_display_socPerc_ui8(void) {
    return data_s.socPerc_ui8;
}
