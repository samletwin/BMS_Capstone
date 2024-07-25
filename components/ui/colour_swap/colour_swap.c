#include "colour_swap.h"

// Function to create a color with swapped channels
lv_color_t my_lv_color_make(uint8_t r, uint8_t g, uint8_t b) {
    return lv_color_make(b, g, r);
}

// lv_color_t full_red = my_lv_color_make(31, 0, 0);   // Full red
//     lv_color_t full_green = my_lv_color_make(0, 63, 0); // Full green
//     lv_color_t full_blue = my_lv_color_make(0, 0, 31);  // Full blue

// lv_color_t full_red = my_lv_color_make(31, 0, 0);   // Full red
// lv_color_t full_green = my_lv_color_make(0, 63, 0); // Full green
// lv_color_t full_blue = my_lv_color_make(0, 0, 31);  // Full blue

lv_color_t get_voltage_color(float voltage) {
    float t = (voltage - 3.0f) / (4.2f - 3.0f);  // Normalize voltage to 0-1 range
    t = LV_CLAMP(t, 0.0f, 1.0f);  // Ensure t is between 0 and 1
    uint8_t r = (1.0f - t) * 255;
    uint8_t g = t * 255;
    return my_lv_color_make(r, g, 0);
}