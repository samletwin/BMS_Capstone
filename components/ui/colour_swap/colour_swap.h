#ifndef COLOUR_SWAP_H
#define COLOUR_SWAP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"


// Function to create a color with swapped channels
lv_color_t my_lv_color_make(uint8_t r, uint8_t g, uint8_t b);
lv_color_t get_voltage_color(float voltage);


// extern lv_color_t full_red;   // Full red
// extern lv_color_t full_green; // Full green
// extern lv_color_t full_blue;  // Full blue

#ifdef __cplusplus
} // extern "C"
#endif

#endif // COLOUR_SWAP_H
