#include "SOC.h"

// Define the static const arrays
const float BatterySOC::SOC_TABLE[TABLE_SIZE] = {
    0, 0.00502512562814070, 0.0100502512562814, 0.0150753768844221, 0.0201005025125628,
    0.0251256281407035, 0.0301507537688442, 0.0351758793969849, 0.0402010050251256,
    0.0452261306532663, 0.0552763819095477, 0.0653266331658292, 0.0804020100502513,
    0.0904522613065327, 0.100502512562814, 0.110552763819095, 0.125628140703518,
    0.155778894472362, 0.170854271356784, 0.216080402010050, 0.276381909547739,
    0.296482412060302, 0.326633165829146, 0.346733668341709, 0.402010050251256,
    0.427135678391960, 0.452261306532663, 0.472361809045226, 0.552763819095477,
    0.567839195979900, 0.603015075376884, 0.628140703517588, 0.653266331658292,
    0.703517587939699, 0.723618090452261, 0.743718592964824, 0.788944723618090,
    0.809045226130653, 0.824120603015075, 0.834170854271357, 0.849246231155779,
    0.904522613065327, 0.924623115577890, 0.939698492462312, 0.954773869346734,
    0.964824120603015, 0.974874371859297, 0.984924623115578, 0.994974874371859, 1
};

const float BatterySOC::OCV_TABLE[TABLE_SIZE] = {
    2.50606500000000, 2.70541134123973, 2.81760417353781, 2.89805620877589, 2.96025354599690,
    3.00979099408924, 3.05139068066065, 3.08724874804176, 3.11857037279292, 3.14608817286485,
    3.19237755852895, 3.23007561967661, 3.27939790148126, 3.30931056376016, 3.33570535786419,
    3.35713178775930, 3.38358296362730, 3.42937088597665, 3.44628552499160, 3.49020296280636,
    3.55959617508305, 3.57805723035749, 3.60307618371238, 3.61848295836797, 3.65746436166502,
    3.67639094369603, 3.69723006658122, 3.71544649611300, 3.79216347015583, 3.80764014485851,
    3.84706863447781, 3.87174019396256, 3.89304880898118, 3.92898500000000, 3.94653444180267,
    3.96747892476240, 4.02188199772211, 4.04323329485654, 4.05677526559970, 4.06365415585129,
    4.06967310092295, 4.08088476183897, 4.08714904857348, 4.09425789705177, 4.10503575414873,
    4.11549669108686, 4.12973985963703, 4.14922684045340, 4.17557066349685, 4.19316500000000
};

BatterySOC::BatterySOC() : soc(0), capacity(0), seriesCells(0), parallelCells(0), lastUpdateTime(0) {}

void BatterySOC::initialize(float initialVoltage, int numSeriesCells, int numParallelCells, float cellCapacity) {
    seriesCells = numSeriesCells;
    parallelCells = numParallelCells;
    capacity = cellCapacity;

    float cellVoltage = initialVoltage / seriesCells;
    soc = lookupSOC(cellVoltage);

    lastUpdateTime = millis();
}

void BatterySOC::update(float current) {
    unsigned long currentTime = millis();
    float deltaSeconds = (currentTime - lastUpdateTime) / 1000.0f;

    // Calculate change in SOC
    float deltaSoc = (current * deltaSeconds) / (capacity * 3600 * parallelCells);

    // Update SOC
    soc -= deltaSoc;  // Subtract because positive current means discharging
    soc = constrain(soc, 0.0f, 1.0f);

    lastUpdateTime = currentTime;
}

float BatterySOC::getSOC() const {
    return soc;
}

int BatterySOC::binarySearch(float ocv) const {
    int low = 0;
    int high = TABLE_SIZE - 1;

    while (low <= high) {
        int mid = (low + high) / 2;
        if (OCV_TABLE[mid] == ocv) {
            return mid;
        } else if (OCV_TABLE[mid] < ocv) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }

    return low > 0 ? low - 1 : 0;
}

float BatterySOC::interpolate(float x, float x1, float y1, float x2, float y2) const {
    return y1 + (x - x1) * (y2 - y1) / (x2 - x1);
}

float BatterySOC::lookupSOC(float ocv) const {
    int index = binarySearch(ocv);
    
    if (index >= TABLE_SIZE - 1) {
        return SOC_TABLE[TABLE_SIZE - 1];
    }
    
    float x1 = OCV_TABLE[index];
    float x2 = OCV_TABLE[index + 1];
    float y1 = SOC_TABLE[index];
    float y2 = SOC_TABLE[index + 1];
    
    return interpolate(ocv, x1, y1, x2, y2);
}