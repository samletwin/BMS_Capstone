#include "soh.h"
#include "esp_log.h"
#include "esp_dsp.h"

#define TAG "SOH"

void SOH::printMat(const dspm::Mat* mat) {
    if (mat == nullptr) {
        printf("Error: Null pointer to Mat object.\n");
        return;
    }
    for (int i = 0; i < mat->length; ++i) {
        printf("%f ", mat->data[i]);
    }
    printf("\n");
}

SOH::SOH() {
    // Initialize P as a large value to indicate initial uncertainty
    P = dspm::Mat(2, 2);
    P(0,0) = 1e6f;
    P(0,1) = 0.0f;
    P(1,0) = 0.0f;
    P(1,1) = 1e6f;

    // Initialize phi as zero
    phi = dspm::Mat(2, 1);
    phi(0,0) = 0.0f;
    phi(1,0) = 0.0f;
}

soh_result SOH::LeastSquares(const int16_t *current_mA, const uint16_t *voltage_mV, uint16_t size, bool debug) {
    dspm::Mat A(size, 2);
        dspm::Mat B(size, 1);
        for (uint16_t i = 0; i < size; i++) {
            A(i,0) = 1;
            A(i,1) = (float)current_mA[i];
        }
        for (uint16_t i = 0; i < size; i++) {
            B(i,0) = (float)voltage_mV[i];
        }
        dspm::Mat A_t = A.t();
        dspm::Mat result_1(A_t.rows, A.cols);
        dspm::Mat result_1_inv(A_t.rows, A.cols);
        dspm_mult_ex_f32_ansi(A_t.data, A.data, result_1.data, A_t.rows, A.rows, A.cols, A_t.padding, A.padding, result_1.padding);
        result_1_inv = result_1.inverse();
        dspm::Mat result_2(result_1_inv.rows, A_t.cols);
        dspm_mult_ex_f32_ansi(result_1_inv.data, A_t.data, result_2.data, result_1_inv.rows, A_t.rows, A_t.cols, result_1_inv.padding, A_t.padding, result_2.padding);
        dspm::Mat result_f(result_2.rows, B.cols);
        dspm_mult_ex_f32_ansi(result_2.data, B.data, result_f.data, result_2.rows, B.rows, B.cols, result_2.padding, B.padding, result_f.padding);

        if (debug) {
            ESP_LOGD(TAG, "A Matrix:");
            printMat(&A);
            ESP_LOGD(TAG, "B Matrix:");
            printMat(&B);
            ESP_LOGD(TAG, "A_t Matrix:");
            printMat(&A_t);
            ESP_LOGD(TAG, "Step 1 Matrix:");
            printMat(&result_1);
            ESP_LOGD(TAG, "Step 1 Matrix Inv:");
            printMat(&result_1_inv);
            ESP_LOGD(TAG, "Step 2 Matrix:");
            printMat(&result_2);
            ESP_LOGD(TAG, "Final Matrix:");
            printMat(&result_f);
        }

        soh_result retVal = {
            .OCV_f32 = result_f(0,0),
            .internalResistance_f32 = result_f(1,0)
        };
       
        ESP_LOGI(TAG, "Calculated OCV: %.2f, Calculated Internal Resistance: %.2f", retVal.OCV_f32, retVal.internalResistance_f32);
        return retVal;
}

soh_result SOH::MovingLeastSquares(float current_A, float voltage_V) {
    // Construct new data point
    dspm::Mat A(2, 1);
    A(0,0) = 1.0f;
    A(1,0) = current_A;

    // Calculate Kalman gain
    dspm::Mat PA(2, 1);
    dspm_mult_f32_ansi(P.data, A.data, PA.data, 2, 2, 1);

    float APA = A(0,0) * PA(0,0) + A(1,0) * PA(1,0) + 1.0f;
    
    dspm::Mat K(2, 1);
    K(0,0) = PA(0,0) / APA;
    K(1,0) = PA(1,0) / APA;

    // Update phi
    float y = voltage_V - (A(0,0) * phi(0,0) + A(1,0) * phi(1,0));
    phi(0,0) += K(0,0) * y;
    phi(1,0) += K(1,0) * y;

    // Update P
    dspm::Mat I(2, 2);
    I(0,0) = 1.0f; I(0,1) = 0.0f;
    I(1,0) = 0.0f; I(1,1) = 1.0f;

    dspm::Mat KA(2, 2);
    dspm_mult_f32_ansi(K.data, A.data, KA.data, 2, 1, 2);

    dspm::Mat I_KA(2, 2);
    for (int i = 0; i < 4; ++i) {
        I_KA.data[i] = I.data[i] - KA.data[i];
    }

    dspm::Mat P_new(2, 2);
    dspm_mult_f32_ansi(I_KA.data, P.data, P_new.data, 2, 2, 2);
    P = P_new;

    soh_result retVal = {
        .OCV_f32 = phi(0,0),
        .internalResistance_f32 = phi(1,0)
    };
    
    ESP_LOGI(TAG, "Calculated OCV: %.2f, Calculated Internal Resistance: %.2f", retVal.OCV_f32, retVal.internalResistance_f32);
    return retVal;
}