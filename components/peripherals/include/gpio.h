#ifndef GPIO_H
#define GPIO_H

#include "driver/gpio.h"

// Define GPIO pins to be used
#define GPIO_SWITCH_BATTERY_DISCHARGE_OUT GPIO_NUM_5

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    GPIO_LOW = 0,
    GPIO_HIGH = 1
} gpioLevelType;

// Function prototypes
void gpio_init(void);
gpioLevelType gpio_invert_level(gpioLevelType level);
void gpio_set_battery_discharge_switch(gpioLevelType level, bool debug);
/**
 * @brief 
 * 
 * @param debug 
 * @return gpioLevelType Level switch is set to (HIGH/LOW)
 */
gpioLevelType gpio_toggle_discharge_switch(bool debug);

#ifdef __cplusplus
}
#endif

#endif // GPIO_H
