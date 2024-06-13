#ifndef SOH_H
#define SOH_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct {
    float OCV_f32;
    float internalResistance_f32;
} soh_result;

soh_result soh_LeastSquares(const uint16_t current_mA[], const uint16_t voltage_mV[], uint16_t size, bool debug);

#ifdef __cplusplus
}
#endif

#endif /* SOH_H */
