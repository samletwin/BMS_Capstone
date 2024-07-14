#ifndef MCP320X_HELPER_H
#define MCP320X_HELPER_H

#include "driver/gpio.h"
#include "mcp320x.h"

/* CONFIG */
#define VREF_MV 5000
#define CS_PIN GPIO_NUM_2
#define BATT_CURRENT_CHANNEL MCP3202::Channel::SINGLE_0
#define BATT_VOLTAGE_CHANNEL MCP3202::Channel::SINGLE_1

#define R1 10000
#define R2 20000
#define BATT_VOLTAGE_SCALE (R1+R2)/R2
#define CURRENT_SCALE (1000/400) /* 400mV/A scaling - divide by 400 to get A, multiply by 1000 to get mA*/

void mcp320x_init();
uint16_t mcp320x_readBattCurrent_analog_mA(bool debug);
uint16_t mcp320x_readBattVoltage_analog_mV(bool debug);


#endif /* MCP320X_HELPER_H */