#ifndef SOC_H
#define SOC_H

#include <Arduino.h>

#define TABLE_SIZE 50

class BatterySOC {
private:
    float soc;                 // State of Charge (0.0 to 1.0)
    float capacity;            // Capacity of a single cell (Ah)
    int seriesCells;           // Number of cells in series
    int parallelCells;         // Number of cells in parallel
    unsigned long lastUpdateTime;  // Last update time in milliseconds
    
    static const float SOC_TABLE[TABLE_SIZE];
    static const float OCV_TABLE[TABLE_SIZE];

    int binarySearch(float ocv) const;
    float interpolate(float x, float x1, float y1, float x2, float y2) const;

public:
    BatterySOC();

    void initialize(float initialVoltage, int numSeriesCells, int numParallelCells, float cellCapacity);
    void update(float current);
    float getSOC() const;
    float lookupSOC(float ocv) const;
};

#endif // BATTERY_SOC_H