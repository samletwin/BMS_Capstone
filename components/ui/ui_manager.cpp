#include "ui_manager.h"

UIManager::UIManager() : 
    packVoltage(0), packCurrent(0), bmsSoc(0), mySoc(0),
    numberOfCells(0), bmsCapacitymAh(0), myCapacitymAh(0),
    cellTemperature(0), timeToShutdown(0) {}

UIManager::~UIManager() {}

void UIManager::updatePackVoltage(float voltage) { packVoltage = voltage; }
void UIManager::updatePackCurrent(float current) { packCurrent = current; }
void UIManager::updateBmsSoc(float soc) { bmsSoc = soc; }
void UIManager::updateMySoc(float soc) { mySoc = soc; }
void UIManager::updateNumberOfCells(uint8_t cellCount) { numberOfCells = cellCount; }
void UIManager::updateCellVoltages(const std::vector<float>& voltages) { cellVoltages = voltages; }
void UIManager::updateBmsCapacity(int capacitymAh) { bmsCapacitymAh = capacitymAh; }
void UIManager::updateMyCapacity(int capacitymAh) { myCapacitymAh = capacitymAh; }
void UIManager::updateCellTemperature(int temperature) { cellTemperature = temperature; }
void UIManager::updateTimeToShutdown(float time) { timeToShutdown = time; }

float UIManager::getPackVoltage() const { return packVoltage; }
float UIManager::getPackCurrent() const { return packCurrent; }
float UIManager::getBmsSoc() const { return bmsSoc; }
float UIManager::getMySoc() const { return mySoc; }
uint8_t UIManager::getNumberOfCells() const { return numberOfCells; }
const std::vector<float>& UIManager::getCellVoltages() const { return cellVoltages; }
int UIManager::getBmsCapacity() const { return bmsCapacitymAh; }
int UIManager::getMyCapacity() const { return myCapacitymAh; }
int UIManager::getCellTemperature() const { return cellTemperature; }
float UIManager::getTimeToShutdown() const { return timeToShutdown; }