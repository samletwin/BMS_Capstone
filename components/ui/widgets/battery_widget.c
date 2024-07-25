#include "battery_widget.h"
#include <stdlib.h>
#include "esp_log.h"
#include "colour_swap.h"

static const char *TAG = "BATTERY_WIDGET";

static lv_color_t get_battery_color(int32_t value)
{
    if (value > 75) {
        return my_lv_color_make(0, 63, 0);    
    } else if (value > 50) {
        return my_lv_color_make(31, 0, 0);    
    } else if (value > 25) {
        return my_lv_color_make(31, 32, 0);    
    } else {
        return my_lv_color_make(0, 0, 31); 
    }
}

static void update_battery_widget(battery_widget_t *battery, int32_t value)
{
    char buf[32];
    if (battery->label_text) {
        // lv_snprintf(buf, sizeof(buf), "%s: %d%%", battery->label_text, value);
        lv_label_set_text(battery->label, battery->label_text);
    }
    
    // Update the value label
    lv_snprintf(buf, sizeof(buf), "%d%%", value);
    lv_label_set_text(battery->value_label, buf);
    
    lv_bar_set_value(battery->bar, value, LV_ANIM_OFF);
    
    // Update the color of the battery foreground
    lv_obj_set_style_bg_color(battery->bar, get_battery_color(value), LV_PART_INDICATOR);
    
    ESP_LOGI(TAG, "Battery updated: %s", buf);
}

battery_widget_t* battery_widget_create(lv_obj_t *parent, bool is_vertical, const char *label_text)
{
    battery_widget_t *battery = malloc(sizeof(battery_widget_t));
    if (!battery) {
        ESP_LOGE(TAG, "Failed to allocate memory for battery widget");
        return NULL;
    }
    battery->is_vertical = is_vertical;
    
    battery->label = lv_label_create(parent);
    
    battery->bar = lv_bar_create(parent);
    lv_obj_set_size(battery->bar, is_vertical ? 30 : 100, is_vertical ? 100 : 30);
    
    // Set the outer radius of the bar
    lv_obj_set_style_radius(battery->bar, 10, LV_PART_MAIN);
    
    // Remove the radius of the indicator (interior) part
    lv_obj_set_style_radius(battery->bar, 0, LV_PART_INDICATOR);
    
    // Set the background of the bar to be transparent
    lv_obj_set_style_bg_opa(battery->bar, LV_OPA_TRANSP, LV_PART_MAIN);
    
    // Add a border to the main part to make it look like a battery
    lv_obj_set_style_border_width(battery->bar, 2, LV_PART_MAIN);
    lv_obj_set_style_border_color(battery->bar, lv_color_black(), LV_PART_MAIN);
    
    lv_bar_set_range(battery->bar, 0, 100);
    
    // Create the value label
    battery->value_label = lv_label_create(parent);
    lv_obj_set_style_text_font(battery->value_label, &lv_font_montserrat_16, 0);  // Set font size to 16
    lv_obj_set_style_text_color(battery->value_label, lv_color_black(), 0);  // Set text color to black
    lv_obj_align_to(battery->value_label, battery->bar, LV_ALIGN_CENTER, 0, 20);  // Center the label on the bar
    
    // Align the main label above the bar
    lv_obj_align_to(battery->label, battery->bar, LV_ALIGN_OUT_TOP_MID, 0, -5);
    
    if (label_text) {
        battery->label_text = strdup(label_text);
        ESP_LOGI(TAG, "Battery widget created with label: %s", label_text);
    } else {
        battery->label_text = NULL;
        ESP_LOGI(TAG, "Battery widget created without label");
    }
    
    update_battery_widget(battery, 0);  // Initialize with 0%
    
    ESP_LOGI(TAG, "Battery widget created successfully");
    return battery;
}

void battery_widget_set_value(battery_widget_t *battery, int32_t value)
{
    if (!battery) return;
    update_battery_widget(battery, value);
    ESP_LOGI(TAG, "Battery widget value set to %ld", value);
}

void battery_widget_delete(battery_widget_t *battery)
{
    if (!battery) return;
    if (battery->label_text) {
        free(battery->label_text);
    }
    lv_obj_del(battery->bar);
    lv_obj_del(battery->label);
    lv_obj_del(battery->value_label);
    free(battery);
    ESP_LOGI(TAG, "Battery widget deleted");
}