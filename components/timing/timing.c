#include "esp_log.h"
#include "timing.h"

#define TIMER_DIVIDER         16  // Hardware timer clock divider
#define TIMER_SCALE           (TIMER_BASE_CLK / TIMER_DIVIDER)  // Convert counter value to seconds
static const char *TAG = "TIMING";

// Timer variables
static timer_group_t timer_group = TIMER_GROUP_0;
static timer_idx_t timer_idx = TIMER_0;
static bool timer_is_init = false;

static inline void timer_checkInit() {
    if (false == timer_is_init) 
        ESP_LOGE(TAG, "Attempting to use timer when it is not initialized!");
}

void timer_initialize() {
    if (true == timer_is_init) {
        ESP_LOGI(TAG, "Attempting to init timer when it is already initialized.");
        return;
    }
    else {
        timer_config_t config = {
            .divider = TIMER_DIVIDER,
            .counter_dir = TIMER_COUNT_UP,
            .counter_en = TIMER_PAUSE,
            .alarm_en = TIMER_ALARM_DIS,
            .auto_reload = false,
        };
        ESP_ERROR_CHECK(timer_init(timer_group, timer_idx, &config));
        ESP_ERROR_CHECK(timer_set_counter_value(timer_group, timer_idx, 0x00000000ULL));
        ESP_ERROR_CHECK(timer_enable_intr(timer_group, timer_idx));
        timer_is_init = true;
        ESP_LOGI(TAG, "Timer initialized successfully");
    }
}

void start_timer() {
    timer_checkInit();
    ESP_ERROR_CHECK(timer_set_counter_value(timer_group, timer_idx, 0x00000000ULL));  // Reset counter to zero
    ESP_ERROR_CHECK(timer_start(timer_group, timer_idx));
    ESP_LOGI(TAG, "Timer reset and started");
}

void stop_timer() {
    timer_checkInit();
    ESP_ERROR_CHECK(timer_pause(timer_group, timer_idx));
    ESP_LOGI(TAG, "Timer stopped");
}

uint16_t get_timer_value_ms() {
    timer_checkInit();
    uint64_t timer_val;
    ESP_ERROR_CHECK(timer_get_counter_value(timer_group, timer_idx, &timer_val));
    return (uint16_t)(timer_val / (TIMER_SCALE / 1000));
}

// Function to create and start a periodic timer
// TODO: make this better
esp_err_t create_periodic_timer(periodicTimerType* periodic_timer_ps) {
    if (NULL == periodic_timer_ps) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_timer_create_args_t timer_args = {
        .callback = periodic_timer_ps->user_callback,
        .name = periodic_timer_ps->timer_name,
        .arg = periodic_timer_ps
    };

    // Create the timer
    esp_err_t err = esp_timer_create(&timer_args, &periodic_timer_ps->timer_handle);
    if (ESP_OK != err) {
        ESP_LOGE("TIMER", "Failed to create timer: %s", esp_err_to_name(err));
        return err;
    }

    // Start the timer
    err = esp_timer_start_periodic(periodic_timer_ps->timer_handle, periodic_timer_ps->timer_interval_us_ui64);
    if (ESP_OK != err) {
        ESP_LOGE("TIMER", "Failed to start timer: %s", esp_err_to_name(err));
        esp_timer_delete(periodic_timer_ps->timer_handle);
    }

    return err;
}


// Function to delete the periodic timer
esp_err_t delete_periodic_timer(periodicTimerType* periodic_timer_ps) {
    if (NULL == periodic_timer_ps) {
        return ESP_ERR_INVALID_ARG;
    }

    // Stop the timer
    esp_err_t err = esp_timer_stop(periodic_timer_ps->timer_handle);
    if (err != ESP_OK) {
        ESP_LOGE("TIMER", "Failed to stop timer: %s", esp_err_to_name(err));
        return err;
    }

    // Delete the timer
    err = esp_timer_delete(periodic_timer_ps->timer_handle);
    if (err != ESP_OK) {
        ESP_LOGE("TIMER", "Failed to delete timer: %s", esp_err_to_name(err));
        return err;
    }

    // Clear the timer handle
    periodic_timer_ps = NULL;

    return ESP_OK;
}