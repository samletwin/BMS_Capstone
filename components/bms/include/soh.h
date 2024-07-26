#ifndef SOH_H
#define SOH_H

#include <stdint.h>
#include "esp_dsp.h"

struct soh_result {
    float OCV_f32;
    float internalResistance_f32;
};

class SOH {
private:
    void printMat(const dspm::Mat* mat);
    dspm::Mat P;  // Covariance matrix
    dspm::Mat phi;  // State vector [OCV, R_internal]

public:
    SOH();

    soh_result LeastSquares(const int16_t *current_mA, const uint16_t *voltage_mV, uint16_t size, bool debug);
    soh_result MovingLeastSquares(float current_mA, float voltage_mV);
};

#endif // SOH_H