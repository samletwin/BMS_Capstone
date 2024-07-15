
#ifndef MCP3202_H
#define MCP3202_H

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_err.h"
#include "stdbool.h"

// MCP3202 channels
typedef enum {
  MCP3202_CHANNEL_0 = 0,
  MCP3202_CHANNEL_1 = 1
} mcp3202_channel_t;

// MCP3202 modes
typedef enum {
  MCP3202_MODE_SINGLE_ENDED = 1,
  MCP3202_MODE_DIFFERENTIAL = 0
} mcp3202_mode_t;

// Function prototypes
esp_err_t mcp3202_init();
uint16_t mcp3202_readBatteryVoltage_mA_ui16(bool debug);
int16_t mcp3202_readBatteryCurrent_mA_si16(bool debug);

#ifdef __cplusplus
}
#endif

#endif // MCP3202_H



