#include "SOC.h"

#include "esp_log.h"
#define TAG "SOC"
// Define the static const arrays
const float BatterySOC::SOC_TABLE[TABLE_SIZE] = {
    0.0, 0.00502512562814070, 0.0100502512562814, 0.0201005025125628, 0.0301507537688442, 
    0.0402010050251256, 0.0502512562814070, 0.0753768844221106, 0.0904522613065327, 0.110552763819095, 
    0.140703517587940, 0.216080402010050, 0.261306532663317, 0.311557788944724, 0.412060301507538,
 0.447236180904523, 0.487437185929648, 0.547738693467337, 0.603015075376884, 0.633165829145729, 
 0.713567839195980, 0.743718592964824, 0.804020100502513, 0.829145728643216, 0.849246231155779, 
 0.914572864321608, 0.944723618090452, 0.964824120603015, 0.984924623115578, 1.0
};

const float BatterySOC::OCV_TABLE[TABLE_SIZE] = {
    2.70485000000000, 2.80721222423896, 2.88496688521856, 3.00087149246144, 3.08150617537700,
    3.14288080607907, 3.18904088966633, 3.28110638862054, 3.32801906183554, 3.37240495680364,
    3.42497976296682, 3.49758107175784, 3.55507354040578, 3.59718330168345, 3.66199910960777,
    3.68872072459907, 3.72441648701938, 3.78243504278330, 3.84435214360726, 3.87289151487165,
    3.93350813254141, 3.96287535535949, 4.03416506187643, 4.05854511705416, 4.07109447631067,
    4.09008343461809, 4.10469541647566, 4.12187410179856, 4.15156457773534, 4.18775000000000
};

BatterySOC::BatterySOC() : soc(0), capacity(0), seriesCells(0), parallelCells(0), lastUpdateTime(0) {
    ESP_LOGD(TAG, "BatterySOC constructor called");
}

void BatterySOC::initialize(float initialVoltage, int numSeriesCells, int numParallelCells, float cellCapacity) {
    seriesCells = numSeriesCells;
    parallelCells = numParallelCells;
    capacity = cellCapacity;
    soc = lookupSOC(initialVoltage);
    lastUpdateTime = millis();
    ESP_LOGD(TAG, "Initialized: initialVoltage=%.4f, seriesCells=%d, parallelCells=%d, capacity=%.4f, initVoltage=%.4f, initial SOC=%.4f",
             initialVoltage, seriesCells, parallelCells, capacity, initialVoltage, soc);
}

void BatterySOC::update(float current) {
    unsigned long currentTime = millis();
    float deltaSeconds = (currentTime - lastUpdateTime) / 1000.0f;
    float deltaSoc = (current * deltaSeconds) / (capacity * 3600 * parallelCells);
    float oldSoc = soc;
    soc -= deltaSoc;
    soc = constrain(soc, 0.0f, 1.0f);
    lastUpdateTime = currentTime;
    ESP_LOGD(TAG, "Update: current=%.4f, deltaSeconds=%.4f, deltaSoc=%.6f, oldSoc=%.4f, newSoc=%.4f",
             current, deltaSeconds, deltaSoc, oldSoc, soc);
}

float BatterySOC::getSOC() const {
    ESP_LOGD(TAG, "getSOC called, returning SOC=%.4f", soc);
    return soc;
}

int BatterySOC::binarySearch(float ocv) const {
    int low = 0;
    int high = TABLE_SIZE - 1;
    float val;
    ESP_LOGD(TAG, "binarySearch started with ocv=%.4f", ocv);
    while (low <= high) {
        int mid = (low + high) / 2;
        val = OCV_TABLE[mid] * seriesCells;
        ESP_LOGD(TAG, "binarySearch: low=%d, high=%d, mid=%d, val=%.4f", low, high, mid, val);
        if (val == ocv) {
            ESP_LOGD(TAG, "binarySearch: exact match found at index %d", mid);
            return mid;
        } else if (val < ocv) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    int result = low > 0 ? low - 1 : 0;
    ESP_LOGD(TAG, "binarySearch: finished, returning index %d", result);
    return result;
}

float BatterySOC::interpolate(float x, float x1, float y1, float x2, float y2) const {
    float result = y1 + (x - x1) * (y2 - y1) / (x2 - x1);
    ESP_LOGD(TAG, "interpolate: x=%.4f, x1=%.4f, y1=%.4f, x2=%.4f, y2=%.4f, result=%.4f",
             x, x1, y1, x2, y2, result);
    return result;
}

float BatterySOC::lookupSOC(float ocv) const {
    ESP_LOGD(TAG, "lookupSOC started with ocv=%.4f", ocv);
    int index = binarySearch(ocv);
   
    if (index >= TABLE_SIZE - 1) {
        ESP_LOGD(TAG, "lookupSOC: index %d >= TABLE_SIZE-1, returning %.4f", index, SOC_TABLE[TABLE_SIZE - 1]);
        return SOC_TABLE[TABLE_SIZE - 1];
    }
   
    float x1 = OCV_TABLE[index] * seriesCells;
    float x2 = OCV_TABLE[index + 1] * seriesCells;
    float y1 = SOC_TABLE[index];
    float y2 = SOC_TABLE[index + 1];
   
    float result = interpolate(ocv, x1, y1, x2, y2);
    ESP_LOGD(TAG, "lookupSOC: index=%d, x1=%.4f, x2=%.4f, y1=%.4f, y2=%.4f, result=%.4f",
             index, x1, x2, y1, y2, result);
    return result;
}