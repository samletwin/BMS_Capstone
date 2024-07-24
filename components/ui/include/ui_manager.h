#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <vector>
#include <stdint.h>

class UIManager {
public:
    UIManager();
    ~UIManager();

    void updatePackVoltage(float voltage);
    void updatePackCurrent(float current);
    void updateBmsSoc(float soc);
    void updateMySoc(float soc);
    void updateNumberOfCells(uint8_t cellCount);
    void updateCellVoltages(const std::vector<float>& voltages);
    void updateBmsCapacity(int capacitymAh);
    void updateMyCapacity(int capacitymAh);
    void updateCellTemperature(int temperature);
    void updateTimeToShutdown(float time);

    float getPackVoltage() const;
    float getPackCurrent() const;
    float getBmsSoc() const;
    float getMySoc() const;
    uint8_t getNumberOfCells() const;
    const std::vector<float>& getCellVoltages() const;
    int getBmsCapacity() const;
    int getMyCapacity() const;
    int getCellTemperature() const;
    float getTimeToShutdown() const;

private:
    float packVoltage;
    float packCurrent;
    float bmsSoc;
    float mySoc;
    uint8_t numberOfCells;
    std::vector<float> cellVoltages;
    int bmsCapacitymAh;
    int myCapacitymAh;
    int cellTemperature;
    float timeToShutdown;
};

#endif // UI_MANAGER_H