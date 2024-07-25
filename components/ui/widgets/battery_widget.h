#ifndef BATTERY_WIDGET_H
#define BATTERY_WIDGET_H

// #ifdef __cplusplus
// extern "C" {
// #endif

#include "lvgl.h"

typedef struct {
    lv_obj_t *bar;
    lv_obj_t *label;
    lv_obj_t *value_label;  // New label for displaying the value
    bool is_vertical;
    char *label_text;
} battery_widget_t;

battery_widget_t* battery_widget_create(lv_obj_t *parent, bool is_vertical, const char *label_text);
void battery_widget_set_value(battery_widget_t *battery, int32_t value);
void battery_widget_delete(battery_widget_t *battery);


// #ifdef __cplusplus
// } /* extern C */
// #endif

#endif // BATTERY_WIDGET_H
