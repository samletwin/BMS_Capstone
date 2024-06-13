#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

/* Littlevgl specific */
#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _objects_t {
    /* screens */
    lv_obj_t *main_single_cell;
    lv_obj_t *settings;
    lv_obj_t *keypad_screen;
    lv_obj_t *keyboard;
    lv_obj_t *key_text_area;
    lv_obj_t *dropdown_menu;
    lv_obj_t *lbl_edit_variable;
    lv_obj_t *btn_settings;
    lv_obj_t *btn_settings_1;
    lv_obj_t *btn_start_soh_measurement;
    lv_obj_t *lbl_bat_current;
    lv_obj_t *lbl_bat_current_variable;
    lv_obj_t *lbl_bat_ocv_variable;
    lv_obj_t *lbl_bat_res_variable;
    lv_obj_t *lbl_bat_tts_variable;
    lv_obj_t *lbl_bat_volt_variable;
    lv_obj_t *lbl_bat_voltage;
    lv_obj_t *lbl_battery_ocv;
    lv_obj_t *lbl_battery_res;
    lv_obj_t *lbl_battery_tts;
    lv_obj_t *lbl_discharge_bat;
    lv_obj_t *lbl_raw_measurements;
    lv_obj_t *lbl_return_btn;
    lv_obj_t *lbl_settings_btn;
    lv_obj_t *lbl_single_cell_title;
    lv_obj_t *lbl_soc;
    lv_obj_t *lbl_soh;
    lv_obj_t *lbl_soh_variable;
    lv_obj_t *lbl_start_soh;
    lv_obj_t *bar_socPerc;
    lv_obj_t *switch_discharge_bat;
    lv_obj_t *lbl_switchDischargeBat;
} objects_t;

extern objects_t objects;

enum ScreensEnum {
    SCREEN_ID_MAIN_SINGLE_CELL = 0,
    SCREEN_ID_SETTINGS = 1,
    SCREEN_ID_KEYPAD_EDIT = 2,
};

void create_screen_main_single_cell();
void tick_screen_main_single_cell();

void create_screen_settings();
void tick_screen_settings();

void create_num_keyboard_screen(void);

void create_screens();
void tick_screen(int screen_index);


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/