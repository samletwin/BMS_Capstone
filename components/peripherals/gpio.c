#include "gpio.h"
#include "esp_log.h"

#define TAG "GPIO_CONTROL"

static bool gpio_init_status = false;

/* Check if init before using any gpio function */
static inline void gpio_checkInit() {
    if (!gpio_init_status)
        ESP_LOGE(TAG, "Error: GPIO has not been initialized!");
}

// Function to initialize the GPIOs
void gpio_init() {
    if (gpio_init_status)
        return;
    else
        gpio_init_status = true;
    // Configure output pins
    gpio_pad_select_gpio(GPIO_SWITCH_BATTERY_DISCHARGE_OUT);
    ESP_ERROR_CHECK(gpio_set_direction(GPIO_SWITCH_BATTERY_DISCHARGE_OUT, GPIO_MODE_OUTPUT));

    ESP_LOGI(TAG, "GPIO initialized");
}

// Function to set GPIO_OUTPUT_PIN_2
void gpio_set_battery_discharge_switch(gpioLevelType level, bool debug) {
    gpio_checkInit();
    ESP_ERROR_CHECK(gpio_set_level(GPIO_SWITCH_BATTERY_DISCHARGE_OUT, (uint32_t)level));
    if (true == debug)
        ESP_LOGD(TAG, "Set battery discharge switch output to %d", level);
}



