// #include "screens.h"
// #include "styles.h"
// #include "ui.h"
// #include "esp_log.h"
// #include "../global_vars.h"
// #include <stdio.h>
// #include <stdlib.h>

// objects_t objects;
// lv_obj_t *tick_value_change_obj;
// typedef void (*set_sohConfig_param)(uint16_t val);

// static set_sohConfig_param set_sohConfig_param_fp;
// static char dropdownText[32];
// static uint8_t selectedVal;
// static void create_keyboard(lv_obj_t* scr, lv_obj_t *ta);

// /* BUTTON EVENT - Discharge battery */
// static void event_handler_cb_main_single_cell_switch_discharge_bat(lv_obj_t *obj, lv_event_t event) {
//     if (event == LV_EVENT_VALUE_CHANGED) {
//         if (false == lv_switch_get_state(obj))
//             set_UI_dischargeBattSwitch_e(UI_SWITCH_OFF);
//         else
//             set_UI_dischargeBattSwitch_e(UI_SWITCH_ON);
//     }
// }


// /* BUTTON EVENT - enter settings screen */
// static void event_handler_cb_main_single_cell_btn_settings(lv_obj_t *obj, lv_event_t event) {
//     if (event == LV_EVENT_PRESSED) {
//         loadScreen(SCREEN_ID_SETTINGS);
//     }
// }

// /* BUTTON EVENT - start soh measurement TODO: disable button until complete */
// static void event_handler_cb_main_single_cell_btn_start_soh_measurement(lv_obj_t *obj, lv_event_t event) {
//     if (event == LV_EVENT_PRESSED && get_UI_sohMeasurementStatus_e() == UI_NO_MEASUREMENT) {
//         set_UI_sohMeasurementStatus_e(UI_START_MEASUREMENT);
//     }
// }

// /* BUTTON EVENT - return to main screen */
// static void event_handler_cb_settings_btn_settings_1(lv_obj_t *obj, lv_event_t event) {
//     if (event == LV_EVENT_PRESSED) {
//         loadScreen(SCREEN_ID_MAIN_SINGLE_CELL);
//     }
// }

// static void event_handler_cb_settings_btn_edit(lv_obj_t *obj, lv_event_t event) {
//     if (event == LV_EVENT_PRESSED) {
//         loadScreen(SCREEN_ID_KEYPAD_EDIT);
//     }
// }

// static void cancel_button_event_handler(lv_obj_t *obj, lv_event_t event) {
//     if (event == LV_EVENT_CLICKED) {
//         loadScreen(SCREEN_ID_SETTINGS);
//     }
// }

// static void confirm_button_event_handler(lv_obj_t *obj, lv_event_t event) {
//     if (event == LV_EVENT_CLICKED) {
//         uint16_t textVal = atoi(lv_textarea_get_text(objects.key_text_area));
//         printf("textVal %u", textVal);
//         if (selectedVal == 0) 
//             set_sohConfig_dischargePeriod_ms_ui16(textVal);
//         else if (selectedVal == 1) 
//             set_sohConfig_numDischarges_ui8(textVal);
//         else if (selectedVal == 2)
//             set_sohConfig_sampleRate_hz_ui16(textVal);
//         loadScreen(SCREEN_ID_SETTINGS);
//     }
// }

// static void ta_event_cb(lv_obj_t * ta_local, lv_event_t e) {
//     if (e == LV_EVENT_CLICKED && objects.keyboard == NULL) {
//         create_keyboard(objects.keypad_screen, objects.key_text_area);
//     }
// }

// static void kb_event_cb(lv_obj_t * keyboard, lv_event_t e)
// {
//     lv_keyboard_def_event_cb(keyboard, e);
//     if(e == LV_EVENT_CANCEL || e == LV_EVENT_APPLY) {
//         if(NULL != objects.keyboard) {
//             lv_keyboard_set_textarea(objects.keyboard, NULL);
//             lv_obj_del(objects.keyboard);
//             objects.keyboard = NULL;
//         }
//     }
// }

// static void event_handler_drop_down(lv_obj_t * obj, lv_event_t event)
// {
//     selectedVal= lv_dropdown_get_selected(obj);
//     printf("Selected Val %u\n",selectedVal);
//     lv_dropdown_get_selected_str(obj, dropdownText, sizeof(dropdownText)); 
//     dropdownText[31] = '\0';
//     printf(dropdownText);
//     if(event == LV_EVENT_VALUE_CHANGED) {
//         // selectedVal= lv_dropdown_get_selected(obj);
//         // printf("Selected Val %u",selectedVal);
//         // lv_dropdown_get_selected_str(obj, dropdownText, sizeof(dropdownText)); 

//     }
// }


// void create_screen_main_single_cell() {
//     // parent 
//     lv_obj_t *parent_obj = lv_obj_create(NULL, NULL);
//     objects.main_single_cell = parent_obj;
//     lv_obj_set_pos(parent_obj, 0, 0);
//     lv_obj_set_size(parent_obj, 480, 320);
//     lv_obj_set_style_local_bg_color(parent_obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
//     // switch_dischargeBat
//     lv_obj_t *switch_discharge_bat = lv_switch_create(parent_obj, NULL);
//     objects.switch_discharge_bat = switch_discharge_bat;
//     lv_obj_set_pos(switch_discharge_bat, 347, 223);
//     lv_obj_set_size(switch_discharge_bat, 50, 25);
//     lv_obj_set_event_cb(switch_discharge_bat, event_handler_cb_main_single_cell_switch_discharge_bat);
//     // lbl_switchDischargeBat
//     lv_obj_t *lbl_switchDischargeBat = lv_label_create(parent_obj, NULL);
//     objects.lbl_switchDischargeBat = lbl_switchDischargeBat;
//     lv_obj_set_pos(lbl_switchDischargeBat, 277, 198);
//     lv_obj_set_style_local_value_align(lbl_switchDischargeBat, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_CENTER);
//     lv_label_set_text(lbl_switchDischargeBat, "Discharge Battery Switch:");


//     // lbl_singleCellTitle
//     lv_obj_t *lbl_single_cell_title = lv_label_create(parent_obj, NULL);
//     objects.lbl_single_cell_title = lbl_single_cell_title;
//     lv_obj_set_pos(lbl_single_cell_title, 22, 17);
//     lv_label_set_text(lbl_single_cell_title, "Single Cell Battery Stats");
//     lv_obj_set_style_local_text_font(lbl_single_cell_title, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_16);
//     // lbl_soc
//     lv_obj_t *lbl_soc = lv_label_create(parent_obj, NULL);
//     objects.lbl_soc = lbl_soc;
//     lv_obj_set_pos(lbl_soc, 56, 61);
//     // lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
//     lv_label_set_text(lbl_soc, "SOC:");
//     // bar_socPerc
//     lv_obj_t *bar_socPerc = lv_bar_create(parent_obj, NULL);
//     objects.bar_socPerc = bar_socPerc;
//     lv_obj_set_pos(bar_socPerc, 98, 48);
//     lv_obj_set_size(bar_socPerc, 105, 42);
//     lv_bar_set_range(bar_socPerc, 0, 100);
//     lv_bar_set_value(bar_socPerc, 30, LV_ANIM_ON);
//     lv_obj_set_style_local_text_color(bar_socPerc, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xffffff));
//     lv_obj_set_style_local_text_opa(bar_socPerc, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, 255);
//     lv_obj_set_style_local_text_font(bar_socPerc, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_14);
//     lv_obj_set_style_local_value_align(bar_socPerc, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_CENTER);
//     lv_obj_set_style_local_bg_color(bar_socPerc, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x000000));
//     lv_obj_set_style_local_bg_opa(bar_socPerc, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, 64);
//     lv_obj_set_style_local_border_color(bar_socPerc, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xffffff));
//     lv_obj_set_style_local_border_width(bar_socPerc, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, 1);
//     // lbl_soh
//     lv_obj_t *lbl_soh = lv_label_create(parent_obj, NULL);
//     objects.lbl_soh = lbl_soh;
//     lv_obj_set_pos(lbl_soh, 75, 101);
//     lv_label_set_text(lbl_soh, "State of Health:");
//     lv_obj_set_style_local_text_color(lbl_soh, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xffffffff));

//     // lbl_sohVariable
//     lv_obj_t *lbl_soh_variable = lv_label_create(parent_obj, NULL);
//     objects.lbl_soh_variable = lbl_soh_variable;
//     lv_obj_set_pos(lbl_soh_variable, 105, 128);
//     lv_label_set_text(lbl_soh_variable, "");
//     lv_obj_set_style_local_text_color(lbl_soh_variable, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xffffcbcb));
//     // lv_obj_set_style_local_pad_top(lbl_soh_variable, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, 0);
//     // lv_obj_set_style_local_pad_left(lbl_soh_variable, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, 20);
//     // lv_obj_set_style_local_pad_right(lbl_soh_variable, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, 20);
//     lv_obj_align(lbl_soh_variable,lbl_soh, LV_ALIGN_CENTER, 0, 128-101);

//     // lbl_batteryRes
//     lv_obj_t *lbl_battery_res = lv_label_create(parent_obj, NULL);
//     objects.lbl_battery_res = lbl_battery_res;
//     lv_obj_set_pos(lbl_battery_res, 31, 155);
//     lv_label_set_text(lbl_battery_res, "Battery Internal Resistance:");
//     lv_obj_set_style_local_text_color(lbl_battery_res, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xffffffff));
//     // lbl_batResVariable
//     lv_obj_t *lbl_bat_res_variable = lv_label_create(parent_obj, NULL);
//     objects.lbl_bat_res_variable = lbl_bat_res_variable;
//     lv_obj_set_pos(lbl_bat_res_variable, 106, 182);
//     lv_label_set_text(lbl_bat_res_variable, "");
//     lv_obj_set_style_local_text_color(lbl_bat_res_variable, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xffffcbcb));
//     // lv_obj_set_style_local_pad_top(lbl_bat_res_variable, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, 0);
//     // lv_obj_set_style_local_pad_left(lbl_bat_res_variable, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, 20);
//     // lv_obj_set_style_local_pad_right(lbl_bat_res_variable, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, 20);
//     lv_obj_align(lbl_bat_res_variable,lbl_battery_res, LV_ALIGN_CENTER, 0, 182-155);


//     // lbl_batteryOcv
//     lv_obj_t *lbl_battery_ocv = lv_label_create(parent_obj, NULL);
//     objects.lbl_battery_ocv = lbl_battery_ocv;
//     lv_obj_set_pos(lbl_battery_ocv, 84, 210);
//     lv_label_set_text(lbl_battery_ocv, "Battery OCV:");
//     lv_obj_set_style_local_text_color(lbl_battery_ocv, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xffffffff));
//     // lbl_batOcvVariable
//     lv_obj_t *lbl_bat_ocv_variable = lv_label_create(parent_obj, NULL);
//     objects.lbl_bat_ocv_variable = lbl_bat_ocv_variable;
//     lv_obj_set_pos(lbl_bat_ocv_variable, 106, 237);
//     lv_label_set_text(lbl_bat_ocv_variable, "");
//     lv_obj_set_style_local_text_color(lbl_bat_ocv_variable, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xffffcbcb));
//     // lv_obj_set_style_local_pad_top(lbl_bat_ocv_variable, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, 0);
//     // lv_obj_set_style_local_pad_left(lbl_bat_ocv_variable, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, 20);
//     // lv_obj_set_style_local_pad_right(lbl_bat_ocv_variable, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, 20);
//     lv_obj_align(lbl_bat_ocv_variable,lbl_battery_ocv, LV_ALIGN_CENTER, 0, 237-210);


//     // lbl_batteryTts
//     lv_obj_t *lbl_battery_tts = lv_label_create(parent_obj, NULL);
//     objects.lbl_battery_tts = lbl_battery_tts;
//     lv_obj_set_pos(lbl_battery_tts, 88, 264);
//     lv_label_set_text(lbl_battery_tts, "Battery TTS:");
//     lv_obj_set_style_local_text_color(lbl_battery_tts, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xffffffff));
//     // lbl_batTtsVariable
//     lv_obj_t *lbl_bat_tts_variable = lv_label_create(parent_obj, NULL);
//     objects.lbl_bat_tts_variable = lbl_bat_tts_variable;
//     lv_obj_set_pos(lbl_bat_tts_variable, 106, 291);
//     lv_label_set_text(lbl_bat_tts_variable, "");
//     lv_obj_set_style_local_text_color(lbl_bat_tts_variable, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xffffcbcb));
//     // lv_obj_set_style_local_pad_top(lbl_bat_tts_variable, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, 0);
//     // lv_obj_set_style_local_pad_left(lbl_bat_tts_variable, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, 20);
//     // lv_obj_set_style_local_pad_right(lbl_bat_tts_variable, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, 20);
//     lv_obj_align(lbl_bat_tts_variable,lbl_battery_tts, LV_ALIGN_CENTER, 0, 291-264);

//     // btn_settings
//     lv_obj_t *btn_settings = lv_btn_create(parent_obj, NULL);
//     objects.btn_settings = btn_settings;
//     lv_obj_set_pos(btn_settings, 374, 6);
//     lv_obj_set_size(btn_settings, 94, 42);
//     lv_obj_set_event_cb(btn_settings, event_handler_cb_main_single_cell_btn_settings);
//     lv_obj_set_style_local_value_str(btn_settings, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, "Settings");
//     lv_obj_set_style_local_value_font(btn_settings, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_16);

//     // lbl_rawMeasurements
//     lv_obj_t *lbl_raw_measurements = lv_label_create(parent_obj, NULL);
//     objects.lbl_raw_measurements = lbl_raw_measurements;
//     lv_obj_set_pos(lbl_raw_measurements, 281, 69);
//     lv_label_set_text(lbl_raw_measurements, "ADC Measurements");
//     lv_obj_set_style_local_text_color(lbl_raw_measurements, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xffffffff));
//     lv_obj_set_style_local_text_opa(lbl_raw_measurements, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
//     lv_obj_set_style_local_text_font(lbl_raw_measurements, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_16);
//     lv_obj_set_style_local_value_align(lbl_raw_measurements, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_LABEL_ALIGN_CENTER);

//     // lbl_batVoltage
//     lv_obj_t *lbl_bat_voltage = lv_label_create(parent_obj, NULL);
//     objects.lbl_bat_voltage = lbl_bat_voltage;
//     lv_obj_set_pos(lbl_bat_voltage, 295, 101);
//     lv_label_set_text(lbl_bat_voltage, "Battery Voltage (mV):");
//     lv_obj_set_style_local_text_color(lbl_bat_voltage, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xffffffff));
//     // lbl_batVoltVariable
//     lv_obj_t *lbl_bat_volt_variable = lv_label_create(parent_obj, NULL);
//     objects.lbl_bat_volt_variable = lbl_bat_volt_variable;
//     lv_obj_set_pos(lbl_bat_volt_variable, 347, 119);
//     lv_label_set_text(lbl_bat_volt_variable, "");
//     lv_obj_set_style_local_text_color(lbl_bat_volt_variable, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xffffcbcb));
//     // lv_obj_set_style_local_pad_top(lbl_bat_volt_variable, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, 0);
//     // lv_obj_set_style_local_pad_left(lbl_bat_volt_variable, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, 20);
//     // lv_obj_set_style_local_pad_right(lbl_bat_volt_variable, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, 20);
//     lv_obj_align(lbl_bat_volt_variable,lbl_bat_voltage, LV_ALIGN_CENTER, 0, 119-101);

//     // lbl_batCurrent
//     lv_obj_t *lbl_bat_current = lv_label_create(parent_obj, NULL);
//     objects.lbl_bat_current = lbl_bat_current;
//     lv_obj_set_pos(lbl_bat_current, 293, 138);
//     lv_label_set_text(lbl_bat_current, "Battery Current (mA):");
//     lv_obj_set_style_local_text_color(lbl_bat_current, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xffffffff));
//     // lbl_batCurrentVariable
//     lv_obj_t *lbl_bat_current_variable = lv_label_create(parent_obj, NULL);
//     objects.lbl_bat_current_variable = lbl_bat_current_variable;
//     lv_obj_set_pos(lbl_bat_current_variable, 347, 154);
//     lv_label_set_text(lbl_bat_current_variable, "");
//     lv_obj_set_style_local_text_color(lbl_bat_current_variable, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xffffcbcb));
//     // lv_obj_set_style_local_pad_top(lbl_bat_current_variable, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, 0);
//     // lv_obj_set_style_local_pad_left(lbl_bat_current_variable, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, 20);
//     // lv_obj_set_style_local_pad_right(lbl_bat_current_variable, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, 20);
//     lv_obj_align(lbl_bat_current_variable,lbl_bat_current, LV_ALIGN_CENTER, 0, 154-138);

//     // btn_startSohMeasurement
//     lv_obj_t *btn_start_soh_measurement = lv_btn_create(parent_obj, NULL);
//     objects.btn_start_soh_measurement = btn_start_soh_measurement;
//     lv_obj_set_pos(btn_start_soh_measurement, 311, 265);
//     lv_obj_set_size(btn_start_soh_measurement, 121, 42);
//     lv_obj_set_event_cb(btn_start_soh_measurement, event_handler_cb_main_single_cell_btn_start_soh_measurement);
//     lv_obj_set_style_local_value_str(btn_start_soh_measurement, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, "SOH Measure");

// }

// // Function to update the label text based on the variable
// void update_label_i(lv_obj_t *label, const char *text, int value) {
//     if (label == NULL) {
//         ESP_LOGE("SCREENS", "Label object is NULL");
//         return;
//     }
//     // ESP_LOGD("SCREENS", "Updating label value to: %i", value);

//     // Ensure LVGL operations are performed in the right context
//     lv_obj_t *lv_label_obj = label; // Ensure correct type casting if necessary
//     lv_label_set_text_fmt(lv_label_obj, text, value);
// }

// // Function to update the label text based on the variable
// void update_label_f(lv_obj_t *label, const char *text, float value) {
//     if (label == NULL) {
//         ESP_LOGE("SCREENS", "Label object is NULL");
//         return;
//     }
//     // ESP_LOGD("SCREENS", "Updating label value to: %i", value);

//     // Ensure LVGL operations are performed in the right context
//     lv_obj_t *lv_label_obj = label; // Ensure correct type casting if necessary
//     lv_label_set_text_fmt(lv_label_obj, text, value);
// }


// void tick_screen_main_single_cell() {
//     const ui_displayDataType* tick_data = get_all_UI_displayData_ps();
//     /* Update bar_socPerc val*/
//     // update_label(&objects.bar_socPerc, "%u %%",tick_data->sohPerc_ui8);
//     update_label_i(objects.lbl_soh_variable, "%u %%", tick_data->sohPerc_ui8);
//     update_label_f(objects.lbl_bat_res_variable, "%f ohms", tick_data->batRes_f32);
//     update_label_f(objects.lbl_bat_ocv_variable, "%f mV", tick_data->batOcv_f32);
//     update_label_i(objects.lbl_bat_tts_variable, "%u sec", tick_data->batTts_ui32);
//     update_label_i(objects.lbl_bat_volt_variable, "%u mV", tick_data->adcBatVolt_mV_ui16);
//     update_label_i(objects.lbl_bat_current_variable, "%u mA", tick_data->adcBatCurrent_mA_si16);
//     // ESP_LOGD("SCREENS","tick data: %f", tick_data->batRes_f32);

// }


// void create_screen_wallpaper() {
//     // Create the main object
//     lv_obj_t *wallpaper = lv_obj_create(NULL, NULL);
//     objects.wallpaper_screen = wallpaper;
//     lv_obj_set_pos(wallpaper, 0, 0);
//     lv_obj_set_size(wallpaper, 480, 320);

//     // Set the background color
//     lv_obj_set_style_local_bg_color(wallpaper, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GREEN);

//     // Create a label with the text
//     lv_obj_t *label = lv_label_create(wallpaper, NULL);
//     lv_label_set_text(label, "E-Bike State of Health Measurement System");
//     lv_obj_set_style_local_text_color(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
//     lv_obj_set_style_local_text_font(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_28); // Adjust the font size as needed

//     // Set the label to wrap text within the bounds of the screen
//     lv_label_set_long_mode(label, LV_LABEL_LONG_BREAK);
//     lv_obj_set_width(label, 480-100);

//     // Align the label to the center
//     lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);
// }

// void create_table(lv_obj_t *scr)
// {
//     /* Create a table on the page */
//     lv_obj_t *table = lv_table_create(scr, NULL);
        
//     /* Set the size of the table */
//     lv_obj_set_size(table, 300, 300); // Set the size of the table

//     /* Set the number of rows and columns */
//     lv_table_set_col_cnt(table, 2);
//     lv_table_set_row_cnt(table, 4);

//     lv_table_set_cell_type(table, 0, 0, LV_TABLE_PART_CELL2);
//     lv_table_set_cell_type(table, 0, 1, LV_TABLE_PART_CELL2);

//     /* Set column widths */
//     lv_table_set_col_width(table, 0, 125);
//     lv_table_set_col_width(table, 1, 100);

//     /* Fill the table with some data */
//     lv_table_set_cell_value_fmt(table, 0, 0, "SOH Settings");
//     lv_table_set_cell_value_fmt(table, 0, 1, "Current Value");
    
//     lv_table_set_cell_value_fmt(table, 1, 0, "Discharge Period (ms)");
//     lv_table_set_cell_value_fmt(table, 1, 1, "%u", get_sohConfig_dischargePeriod_ms_ui16());

//     lv_table_set_cell_value_fmt(table, 2, 0, "# of Discharges");
//     lv_table_set_cell_value_fmt(table, 2, 1, "%u", get_sohConfig_numDischarges_ui8());

//     lv_table_set_cell_value_fmt(table, 3, 0, "Sample Rate (Hz)");
//     lv_table_set_cell_value_fmt(table, 3, 1, "%u", get_sohConfig_sampleRate_hz_ui16());
// }


// void create_screen_settings() {
//     // Create the main object
//     lv_obj_t *settings = lv_obj_create(NULL, NULL);
//     objects.settings = settings;
//     lv_obj_set_pos(settings, 0, 0);
//     lv_obj_set_size(settings, 480, 320);

//     /* Create a page */
//     lv_obj_t *page = lv_page_create(settings, NULL);
//     lv_obj_set_size(page, 480, 320); // Set the size of the page
//     lv_obj_align(page, NULL, LV_ALIGN_CENTER, 0, 0); // Align the page to the center


//     // Create the parent object
//     lv_obj_t *parent_obj = page;

//     // btn_settings_1
//     lv_obj_t *btn_settings_1 = lv_btn_create(parent_obj, NULL);
//     objects.btn_settings_1 = btn_settings_1;
//     lv_obj_align(btn_settings_1, page, LV_ALIGN_IN_TOP_RIGHT, 0, 20);
//     lv_obj_set_size(btn_settings_1, 94, 42);
//     lv_obj_set_event_cb(btn_settings_1, event_handler_cb_settings_btn_settings_1);
    

//     // lbl_returnBtn
//     lv_obj_t *lbl_return_btn = lv_label_create(btn_settings_1, NULL);
//     objects.lbl_return_btn = lbl_return_btn;
//     lv_obj_align(lbl_return_btn, btn_settings_1, LV_ALIGN_CENTER, 0, 0);
//     lv_label_set_text(lbl_return_btn, "Return");


//     lv_obj_t * ddlist = lv_dropdown_create(parent_obj, NULL);
//     objects.dropdown_menu = ddlist;

//     lv_dropdown_set_options(ddlist, "Discharge Period (ms)\n" "# of Discharges\n" "Sample Rate (Hz)");
//     lv_obj_align(ddlist, parent_obj, LV_ALIGN_IN_RIGHT_MID, -20, 0);
//     lv_obj_set_event_cb(ddlist, event_handler_drop_down);
//     strncpy(dropdownText, "Discharge Period (ms)\n", sizeof(dropdownText) - 1);
//     set_sohConfig_param_fp = set_sohConfig_dischargePeriod_ms_ui16;
    
//     lv_obj_t * btn_edit = lv_btn_create(parent_obj, NULL);
//     // objects.btn_settings_1 = btn_settings_1;
//     lv_obj_align(btn_edit, page, LV_ALIGN_IN_BOTTOM_RIGHT, 0, 0);
//     lv_obj_set_size(btn_edit, 94, 42);
//     lv_obj_set_event_cb(btn_edit, event_handler_cb_settings_btn_edit);

//     lv_obj_t *lbl_btnEdit = lv_label_create(btn_edit, NULL);
//     // objects.lbl_btnEdit = lbl_btnEdit;
//     lv_obj_align(lbl_btnEdit, btn_settings_1, LV_ALIGN_CENTER, 0, 0);
//     lv_label_set_text(lbl_btnEdit, "Edit");

//     create_table(parent_obj);
// }

// static void create_keyboard(lv_obj_t* scr, lv_obj_t *ta) {
//     // /* Create a keyboard */
//     lv_obj_t *kb = lv_keyboard_create(scr, NULL);
//     objects.keyboard = kb;
//     lv_keyboard_set_mode(kb, LV_KEYBOARD_MODE_NUM);
//     lv_obj_set_size(kb, 480, 150); // Set the keyboard size to fit the bottom half of the screen
//     lv_obj_align(kb, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
//     lv_obj_set_event_cb(kb, kb_event_cb);
//     lv_keyboard_set_cursor_manage(kb, true);
//     lv_keyboard_set_textarea(kb, ta);
// }

// void create_num_keyboard_screen(void) {
//     /* Create a new screen */
//     lv_obj_t *scr = lv_obj_create(NULL, NULL);
//     objects.keypad_screen = scr;
//     lv_obj_set_pos(scr, 0, 0);
//     lv_obj_set_size(scr, 480, 320);
//     /* Create a text area */
//     lv_obj_t *ta = lv_textarea_create(scr, NULL);
//     objects.key_text_area = ta;
//     lv_obj_set_size(ta, 200, 50); // Adjust the size to fit the screen
//     lv_obj_align(ta, NULL, LV_ALIGN_IN_TOP_MID, 0, 30); // Position at the top middle with some margin
//     lv_textarea_set_text(ta, "");
//     lv_obj_set_event_cb(ta, ta_event_cb);


//     create_keyboard(scr, ta);
//     // /* Focus the text area and disable unfocus */
//     // lv_textarea_set_text(ta, "");
//     // lv_textarea_set_one_line(ta, true);
//     // lv_textarea_set_pwd_mode(ta, false);
//     // lv_textarea_set_text_align(ta, LV_LABEL_ALIGN_CENTER);
//     // lv_textarea_set_cursor_click_pos(ta, false);
//     // lv_obj_set_click(ta, false); // Disable clicking to unfocus


//     lv_obj_t *label_edit = lv_label_create(scr, NULL);
//     lv_obj_set_pos(label_edit, 240, 100);
//     lv_label_set_text(label_edit, "Editing value");
//     lv_obj_t *label_var = lv_label_create(scr, NULL);
//     lv_obj_set_pos(label_var, 240, 125);
//     lv_label_set_text(label_var, &dropdownText);

//     // /* Create a cancel button */
//     lv_obj_t *btn_cancel = lv_btn_create(scr, NULL);
//     lv_obj_set_size(btn_cancel, 80, 50);
//     lv_obj_align(btn_cancel, NULL, LV_ALIGN_IN_TOP_RIGHT, -10, 10);
//     lv_obj_set_event_cb(btn_cancel, cancel_button_event_handler);
//     // /* Add a label to the cancel button */
//     lv_obj_t *label_btnCancel = lv_label_create(btn_cancel, NULL);
//     lv_label_set_text(label_btnCancel, "Cancel");

//     // /* Create a confirm button */
//     lv_obj_t *btn_confirm = lv_btn_create(scr, NULL);
//     lv_obj_set_size(btn_confirm, 80, 50);
//     lv_obj_align(btn_confirm, NULL, LV_ALIGN_IN_TOP_LEFT, -10, 10);
//     lv_obj_set_event_cb(btn_confirm, confirm_button_event_handler);
//     // /* Add a label to the confirm button */
//     lv_obj_t *label_btnConfirm = lv_label_create(btn_confirm, NULL);
//     lv_label_set_text(label_btnConfirm, "Confirm");

//     // /* Make the text area always focused */
//     // lv_group_t *g = lv_group_create();
//     // lv_group_add_obj(g, ta);
//     // lv_group_focus_obj(ta);
//     // lv_indev_set_group(lv_indev_get_act(), g);
// }

// void tick_screen_settings() {
// }


// void create_screens() {
//     create_screen_main_single_cell();
//     create_screen_settings();
//     create_num_keyboard_screen();
//     create_screen_wallpaper();
// }

// typedef void (*tick_screen_func_t)();

// tick_screen_func_t tick_screen_funcs[] = {
//     tick_screen_main_single_cell,
//     tick_screen_settings,
// };

// void tick_screen(int screen_index) {
//     tick_screen_funcs[screen_index]();
// }
