#include "soh.h"
#include "esp_log.h"
#include "esp_dsp.h"

#define TAG "SOH"

void printMat(const dspm::Mat* mat) {
    if (mat == nullptr) {
        printf("Error: Null pointer to Mat object.\n");
        return;
    }

    for (int i = 0; i < mat->length; ++i) {
        printf("%f ", mat->data[i]);
    }
    printf("\n");
}


/* For least squares, we are finding phi, where phi = [OCV; R_internal] 
  Phi = (A.t()*A)^-1 * A.t() * B
  A is a Nx2 matrix, first column all 1s, second column being the Nth current measurement
  B is a Nx1 matrix, with each row being the Nth voltage measurement */
soh_result soh_LeastSquares(const int16_t current_mA[], const uint16_t voltage_mV[], uint16_t size, bool debug) {
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

    /* C[m][k] = A[m][n] * B[n][k] */
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

    // buffer = buffer * A.t() * B;

    soh_result retVal = {
        .OCV_f32 = result_f(0,0),
        .internalResistance_f32 = result_f(1,0)
    };
    
    ESP_LOGI(TAG, "Calculated OCV: %.2f, Calculated Internal Resistance: %.2f", retVal.OCV_f32, retVal.internalResistance_f32);
    return retVal;
}
