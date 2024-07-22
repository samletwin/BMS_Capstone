#include "global_vars.h"

ui_displayDataType data_s = (ui_displayDataType){0};
ui_eventDataType ui_event_data_s = (ui_eventDataType){0};

/* ------------------------------------------------------------------------------------
    SETTER FUNCTIONS
------------------------------------------------------------------------------------- */
/* UI Event Data */
void set_UI_sohMeasurementStatus_e(ui_startMeasurementType val) {
    ui_event_data_s.sohMeasurementStatus_e = val;
}
void set_UI_dischargeBattSwitch_e(ui_switchStatusType val) {
    ui_event_data_s.dischargeBattSwitch_e = val;
}

/* Display Data */
void set_display_packCurrent_mA_si16(int16_t val) {
    data_s.packCurrent_mA_si16 = val;
}
void set_display_packVoltage_mV_ui16(uint16_t val) {
    data_s.packVoltage_mV_ui16 = val;
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
void set_display_socPerc_ui8(uint8_t val) {
    data_s.socPerc_ui8 = val;
}
void set_display_cellVoltage_mV_ui16(uint16_t index, uint16_t val) {
    if (index < 14) {
        data_s.cellVoltage_mV_ui16[index] = val;
    }
}

/* ------------------------------------------------------------------------------------
    GETTER FUNCTIONS
------------------------------------------------------------------------------------- */
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
int16_t get_display_packCurrent_mA_si16(void) {
    return data_s.packCurrent_mA_si16;
}
uint16_t get_display_packVoltage_mV_ui16(void) {
    return data_s.packVoltage_mV_ui16;
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
uint8_t get_display_socPerc_ui8(void) {
    return data_s.socPerc_ui8;
}
uint16_t get_display_cellVoltage_mV_ui16(uint16_t index) {
    if (index < 14) {
        return data_s.cellVoltage_mV_ui16[index];
    }
    return 0; // Or any other error value or handling
}
