#ifndef GPIO_H
#define GPIO_H

#include "driver/gpio.h"

// Define GPIO pins to be used
#define GPIO_SWITCH_BATTERY_DISCHARGE_OUT GPIO_NUM_5

typedef enum {
    GPIO_LOW = 0,
    GPIO_HIGH = 1
} gpioLevelType;

// Function prototypes
void gpio_init(void);
void gpio_set_battery_discharge_switch(gpioLevelType level, bool debug);
void gpio_toggle_soh_timer_callback(void *arg);

#endif // GPIO_H
