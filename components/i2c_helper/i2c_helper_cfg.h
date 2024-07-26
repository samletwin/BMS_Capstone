/**
 * @file i2c_helper_config.h
 * @author Sam Letwin (letwins@uwindsor.ca)
 * @brief 
 * @version 0.1
 * @date 2024-07-24
 * 
 * 
 */

#ifndef I2C_HELPER_CONFIG
#define I2C_HELPER_CONFIG

#ifdef __cplusplus
extern "C" {
#endif

#include "driver/i2c.h"

#define I2C_SDA_PIN 7
#define I2C_SCL_PIN 21
#define I2C_FREQ_HZ 4*1000*100 // 400 kHz
#define I2C_PORT    I2C_NUM_0

#define I2C_SDA_PULLUP  GPIO_PULLUP_ENABLE
#define I2C_SCL_PULLUP  GPIO_PULLUP_ENABLE

#ifdef __cplusplus
} /* extern C */
#endif

#endif /* I2C_HELPER_CONFIG */
