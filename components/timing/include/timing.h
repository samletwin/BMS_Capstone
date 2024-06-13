#ifndef TIMING_H
#define TIMING_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/timer.h"
#include "esp_timer.h"

// Define a type for the callback function
typedef void (*timer_callback_t)(void* arg);

typedef enum {
    TYPE_TOGGLE,
    TYPE_NORMAL
} periodicTimerEnumType;

// Timer structure
// typedef struct {
//     uint16_t *toggle_count_ui16; // Pointer to the toggle counter
//     uint16_t max_toggles_ui16; // Maximum number of toggles
// } periodicTimerToggleType;

// typedef struct {
//     uint16_t timer_callback_count_ui16;
// } periodicTimerNormalType;

typedef struct {
    esp_timer_handle_t timer_handle;
    timer_callback_t user_callback;
    const char* timer_name;
    periodicTimerEnumType timer_type;
    uint64_t timer_interval_us_ui64;
    uint16_t toggle_count_ui16; // Pointer to the toggle counter
    uint16_t max_toggles_ui16; // Maximum number of toggles
} periodicTimerType;





void timer_initialize();
void start_timer();
void stop_timer();
uint16_t get_timer_value_ms();
esp_err_t create_periodic_timer(periodicTimerType* periodic_timer_ps);
esp_err_t delete_periodic_timer(periodicTimerType* periodic_timer_ps);
#endif /* TIMING_H */
