#ifndef MCP3202_CFG_H
#define MCP3202_CFG_H

#include "mcp3202.h"

#define MCP3202_VREF_MV     5000
#define MCP3202_CS_PIN      2

#define BATT_CURRENT_CHANNEL MCP3202_CHANNEL_0
#define BATT_VOLTAGE_CHANNEL MCP3202_CHANNEL_1

#define R1 10000
#define R2 20000
#define BATT_VOLTAGE_SCALE (R1+R2)/R2
#define BATT_CURRENT_SCALE (1000/400) /* 400mV/A scaling - divide by 400 to get A, multiply by 1000 to get mA*/

#endif /* MCP3202_CFG_H */