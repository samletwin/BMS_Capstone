#include "daly_bms_serial.h"
#include "esp_log.h"

// Uncomment the below define to enable debug printing
// #define DEBUG_SERIAL UART_NUM_1

#define UART_PORT_NUM      UART_NUM_1
#define UART_BAUD_RATE     9600
#define UART_TX_PIN        17
#define UART_RX_PIN        16
#define BUF_SIZE           1024

//----------------------------------------------------------------------
// Public Functions
//----------------------------------------------------------------------

Daly_BMS_UART::Daly_BMS_UART()
{
    this->uart_port = UART_PORT_NUM;
}

bool Daly_BMS_UART::Init()
{
    // Initialize the UART interface
    const uart_config_t uart_config = {
        .baud_rate = UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };

    uart_param_config(this->uart_port, &uart_config);
    uart_set_pin(this->uart_port, UART_TX_PIN, UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(this->uart_port, BUF_SIZE, BUF_SIZE, 0, NULL, 0);

    // Set up the output buffer with some values that won't be changing
    this->my_txBuffer[0] = 0xA5; // Start byte
    this->my_txBuffer[1] = 0x40; // Host address
    this->my_txBuffer[3] = 0x08; // Length

    // Fill bytes 5-11 with 0s
    for (uint8_t i = 4; i < 12; i++)
    {
        this->my_txBuffer[i] = 0x00;
    }

    return true;
}

bool Daly_BMS_UART::updateAll()
{
    // Call all get___() functions to populate all members of the "get" struct
    if (!getPackMeasurements()) return false; // 0x90
    if (!getMinMaxCellVoltage()) return false; // 0x91
    if (!getPackTemp()) return false; // 0x92
    if (!getDischargeChargeMosStatus()) return false; // 0x93
    if (!getStatusInfo()) return false; // 0x94
    if (!getCellVoltages()) return false; // 0x95
    if (!getCellTemperature()) return false; // 0x96
    if (!getCellBalanceState()) return false; // 0x97
    if (!getFailureCodes()) return false; // 0x98

    return true;
}

bool Daly_BMS_UART::updateSpecific()
{
    if (!getPackMeasurements()) return false; // 0x90
    if (!getCellVoltages()) return false; // 0x95

    return true;
}

void Daly_BMS_UART::printBmsStats()
{
    // printf("\n%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%d",
    // get.packVoltage, get.packCurrent, get.packSOC,
    // get.cellVmV[0],get.cellVmV[1],get.cellVmV[2],get.cellVmV[3],get.cellVmV[4],get.cellVmV[5],get.cellVmV[6],
    // get.cellVmV[7],get.cellVmV[8],get.cellVmV[9],get.cellVmV[10],get.cellVmV[11],get.cellVmV[12],get.cellVmV[13],
    // get.cellTemperature[0]);

    ESP_LOGI("BMS_STATS", "Current BMS Stats:");
    ESP_LOGI("BMS_STATS", "Pack Voltage: %.1f V", get.packVoltage);
    ESP_LOGI("BMS_STATS", "Pack Current: %.1f A", get.packCurrent);
    ESP_LOGI("BMS_STATS", "Pack SOC: %.1f%%", get.packSOC);

    ESP_LOGI("BMS_STATS", "Max Cell Voltage: %.1f mV (Cell %d)", get.maxCellmV, get.maxCellVNum);
    ESP_LOGI("BMS_STATS", "Min Cell Voltage: %.1f mV (Cell %d)", get.minCellmV, get.minCellVNum);
    ESP_LOGI("BMS_STATS", "Cell Voltage Difference: %.1f mV", get.cellDiff);

    ESP_LOGI("BMS_STATS", "Max Temperature: %d °C", get.tempMax);
    ESP_LOGI("BMS_STATS", "Min Temperature: %d °C", get.tempMin);
    ESP_LOGI("BMS_STATS", "Average Temperature: %.1f °C", get.tempAverage);

    ESP_LOGI("BMS_STATS", "Charge/Discharge Status: %s", get.chargeDischargeStatus.c_str());
    ESP_LOGI("BMS_STATS", "Charge FET State: %s", get.chargeFetState ? "ON" : "OFF");
    ESP_LOGI("BMS_STATS", "Discharge FET State: %s", get.disChargeFetState ? "ON" : "OFF");
    ESP_LOGI("BMS_STATS", "BMS Heartbeat: %d", get.bmsHeartBeat);
    ESP_LOGI("BMS_STATS", "Residual Capacity: %d mAh", get.resCapacitymAh);

    ESP_LOGI("BMS_STATS", "Number of Cells: %d", get.numberOfCells);
    ESP_LOGI("BMS_STATS", "Number of Temperature Sensors: %d", get.numOfTempSensors);
    ESP_LOGI("BMS_STATS", "Charge State: %s", get.chargeState ? "Connected" : "Disconnected");
    ESP_LOGI("BMS_STATS", "Load State: %s", get.loadState ? "Connected" : "Disconnected");

    for (int i = 0; i < 8; i++)
    {
        ESP_LOGI("BMS_STATS", "Digital IO %d: %s", i, get.dIO[i] ? "HIGH" : "LOW");
    }

    ESP_LOGI("BMS_STATS", "BMS Cycles: %d", get.bmsCycles);

    for (int i = 0; i < get.numberOfCells; i++)
    {
        ESP_LOGI("BMS_STATS", "Cell %d Voltage: %.1f mV", i + 1, get.cellVmV[i]);
    }

    for (int i = 0; i < get.numOfTempSensors; i++)
    {
        ESP_LOGI("BMS_STATS", "Temperature Sensor %d: %d °C", i + 1, get.cellTemperature[i]);
    }

    for (int i = 0; i < get.numberOfCells; i++)
    {
        ESP_LOGI("BMS_STATS", "Cell %d Balance State: %s", i + 1, get.cellBalanceState[i] ? "Balanced" : "Unbalanced");
    }

    ESP_LOGI("BMS_STATS", "Cell Balance Active: %s", get.cellBalanceActive ? "Yes" : "No");
}

void Daly_BMS_UART::printBmsAlarms() 
{
    ESP_LOGI("BMS_STATS", "Alarms:");
    ESP_LOGI("BMS_STATS", "Level 1 Cell Voltage Too High: %s", alarm.levelOneCellVoltageTooHigh ? "Yes" : "No");
    ESP_LOGI("BMS_STATS", "Level 2 Cell Voltage Too High: %s", alarm.levelTwoCellVoltageTooHigh ? "Yes" : "No");
    ESP_LOGI("BMS_STATS", "Level 1 Cell Voltage Too Low: %s", alarm.levelOneCellVoltageTooLow ? "Yes" : "No");
    ESP_LOGI("BMS_STATS", "Level 2 Cell Voltage Too Low: %s", alarm.levelTwoCellVoltageTooLow ? "Yes" : "No");
    ESP_LOGI("BMS_STATS", "Level 1 Pack Voltage Too High: %s", alarm.levelOnePackVoltageTooHigh ? "Yes" : "No");
    ESP_LOGI("BMS_STATS", "Level 2 Pack Voltage Too High: %s", alarm.levelTwoPackVoltageTooHigh ? "Yes" : "No");
    ESP_LOGI("BMS_STATS", "Level 1 Pack Voltage Too Low: %s", alarm.levelOnePackVoltageTooLow ? "Yes" : "No");
    ESP_LOGI("BMS_STATS", "Level 2 Pack Voltage Too Low: %s", alarm.levelTwoPackVoltageTooLow ? "Yes" : "No");

    ESP_LOGI("BMS_STATS", "Level 1 Charge Temp Too High: %s", alarm.levelOneChargeTempTooHigh ? "Yes" : "No");
    ESP_LOGI("BMS_STATS", "Level 2 Charge Temp Too High: %s", alarm.levelTwoChargeTempTooHigh ? "Yes" : "No");
    ESP_LOGI("BMS_STATS", "Level 1 Charge Temp Too Low: %s", alarm.levelOneChargeTempTooLow ? "Yes" : "No");
    ESP_LOGI("BMS_STATS", "Level 2 Charge Temp Too Low: %s", alarm.levelTwoChargeTempTooLow ? "Yes" : "No");
    ESP_LOGI("BMS_STATS", "Level 1 Discharge Temp Too High: %s", alarm.levelOneDischargeTempTooHigh ? "Yes" : "No");
    ESP_LOGI("BMS_STATS", "Level 2 Discharge Temp Too High: %s", alarm.levelTwoDischargeTempTooHigh ? "Yes" : "No");
    ESP_LOGI("BMS_STATS", "Level 1 Discharge Temp Too Low: %s", alarm.levelOneDischargeTempTooLow ? "Yes" : "No");
    ESP_LOGI("BMS_STATS", "Level 2 Discharge Temp Too Low: %s", alarm.levelTwoDischargeTempTooLow ? "Yes" : "No");

    ESP_LOGI("BMS_STATS", "Level 1 Charge Current Too High: %s", alarm.levelOneChargeCurrentTooHigh ? "Yes" : "No");
    ESP_LOGI("BMS_STATS", "Level 2 Charge Current Too High: %s", alarm.levelTwoChargeCurrentTooHigh ? "Yes" : "No");
    ESP_LOGI("BMS_STATS", "Level 1 Discharge Current Too High: %s", alarm.levelOneDischargeCurrentTooHigh ? "Yes" : "No");
    ESP_LOGI("BMS_STATS", "Level 2 Discharge Current Too High: %s", alarm.levelTwoDischargeCurrentTooHigh ? "Yes" : "No");
    ESP_LOGI("BMS_STATS", "Level 1 SOC Too High: %s", alarm.levelOneStateOfChargeTooHigh ? "Yes" : "No");
    ESP_LOGI("BMS_STATS", "Level 2 SOC Too High: %s", alarm.levelTwoStateOfChargeTooHigh ? "Yes" : "No");
    ESP_LOGI("BMS_STATS", "Level 1 SOC Too Low: %s", alarm.levelOneStateOfChargeTooLow ? "Yes" : "No");
    ESP_LOGI("BMS_STATS", "Level 2 SOC Too Low: %s", alarm.levelTwoStateOfChargeTooLow ? "Yes" : "No");

    ESP_LOGI("BMS_STATS", "Level 1 Cell Voltage Difference Too High: %s", alarm.levelOneCellVoltageDifferenceTooHigh ? "Yes" : "No");
    ESP_LOGI("BMS_STATS", "Level 2 Cell Voltage Difference Too High: %s", alarm.levelTwoCellVoltageDifferenceTooHigh ? "Yes" : "No");
    ESP_LOGI("BMS_STATS", "Level 1 Temp Sensor Difference Too High: %s", alarm.levelOneTempSensorDifferenceTooHigh ? "Yes" : "No");
    ESP_LOGI("BMS_STATS", "Level 2 Temp Sensor Difference Too High: %s", alarm.levelTwoTempSensorDifferenceTooHigh ? "Yes" : "No");

    ESP_LOGI("BMS_STATS", "Charge FET Temperature Too High: %s", alarm.chargeFETTemperatureTooHigh ? "Yes" : "No");
    ESP_LOGI("BMS_STATS", "Discharge FET Temperature Too High: %s", alarm.dischargeFETTemperatureTooHigh ? "Yes" : "No");
    ESP_LOGI("BMS_STATS", "Failure of Charge FET Temperature Sensor: %s", alarm.failureOfChargeFETTemperatureSensor ? "Yes" : "No");
    ESP_LOGI("BMS_STATS", "Failure of Discharge FET Temperature Sensor: %s", alarm.failureOfDischargeFETTemperatureSensor ? "Yes" : "No");
    ESP_LOGI("BMS_STATS", "Failure of Charge FET Adhesion: %s", alarm.failureOfChargeFETAdhesion ? "Yes" : "No");
    ESP_LOGI("BMS_STATS", "Failure of Discharge FET Adhesion: %s", alarm.failureOfDischargeFETAdhesion ? "Yes" : "No");
    ESP_LOGI("BMS_STATS", "Failure of Charge FET Breaker: %s", alarm.failureOfChargeFETTBreaker ? "Yes" : "No");
    ESP_LOGI("BMS_STATS", "Failure of Discharge FET Breaker: %s", alarm.failureOfDischargeFETBreaker ? "Yes" : "No");

    ESP_LOGI("BMS_STATS", "Failure of AFE Acquisition Module: %s", alarm.failureOfAFEAcquisitionModule ? "Yes" : "No");
    ESP_LOGI("BMS_STATS", "Failure of Voltage Sensor Module: %s", alarm.failureOfVoltageSensorModule ? "Yes" : "No");
    ESP_LOGI("BMS_STATS", "Failure of Temperature Sensor Module: %s", alarm.failureOfTemperatureSensorModule ? "Yes" : "No");
    ESP_LOGI("BMS_STATS", "Failure of EEPROM Storage Module: %s", alarm.failureOfEEPROMStorageModule ? "Yes" : "No");
    ESP_LOGI("BMS_STATS", "Failure of Real-time Clock Module: %s", alarm.failureOfRealtimeClockModule ? "Yes" : "No");
    ESP_LOGI("BMS_STATS", "Failure of Precharge Module: %s", alarm.failureOfPrechargeModule ? "Yes" : "No");
    ESP_LOGI("BMS_STATS", "Failure of Vehicle Communication Module: %s", alarm.failureOfVehicleCommunicationModule ? "Yes" : "No");
    ESP_LOGI("BMS_STATS", "Failure of Intranet Communication Module: %s", alarm.failureOfIntranetCommunicationModule ? "Yes" : "No");

    ESP_LOGI("BMS_STATS", "Failure of Current Sensor Module: %s", alarm.failureOfCurrentSensorModule ? "Yes" : "No");
    ESP_LOGI("BMS_STATS", "Failure of Main Voltage Sensor Module: %s", alarm.failureOfMainVoltageSensorModule ? "Yes" : "No");
    ESP_LOGI("BMS_STATS", "Failure of Short Circuit Protection: %s", alarm.failureOfShortCircuitProtection ? "Yes" : "No");
    ESP_LOGI("BMS_STATS", "Failure of Low Voltage No Charging: %s", alarm.failureOfLowVoltageNoCharging ? "Yes" : "No");
}


bool Daly_BMS_UART::getPackMeasurements() // 0x90
{
    this->sendCommand(COMMAND::VOUT_IOUT_SOC);

    if (!this->receiveBytes())
    {

        ESP_LOGE("DALY-BMS DEBUG", "Receive failed, V, I, & SOC values won't be modified!");

        return false;
    }

    // Pull the relevant values out of the buffer
    get.packVoltage = ((float)((this->my_rxBuffer[4] << 8) | this->my_rxBuffer[5]) / 10.0f);
    // The current measurement is given with a 30000 unit offset (see /docs/)
    get.packCurrent = ((float)(((this->my_rxBuffer[8] << 8) | this->my_rxBuffer[9]) - 30000) / 10.0f);
    get.packSOC = ((float)((this->my_rxBuffer[10] << 8) | this->my_rxBuffer[11]) / 10.0f);

    // ESP_LOGD("DALY-BMS DEBUG", "%fV, %fA, %fSOC", get.packVoltage, get.packCurrent, get.packSOC);

    return true;
}

bool Daly_BMS_UART::getMinMaxCellVoltage() // 0x91
{
    this->sendCommand(COMMAND::MIN_MAX_CELL_VOLTAGE);

    if (!this->receiveBytes())
    {

        ESP_LOGE("DALY-BMS DEBUG", "Receive failed, min/max cell values won't be modified!");

        return false;
    }

    get.maxCellmV = (float)((this->my_rxBuffer[4] << 8) | this->my_rxBuffer[5]);
    get.maxCellVNum = this->my_rxBuffer[6];
    get.minCellmV = (float)((this->my_rxBuffer[7] << 8) | this->my_rxBuffer[8]);
    get.minCellVNum = this->my_rxBuffer[9];
    get.cellDiff = (get.maxCellmV - get.minCellmV);

    return true;
}

bool Daly_BMS_UART::getPackTemp() // 0x92
{
    this->sendCommand(COMMAND::MIN_MAX_TEMPERATURE);

    if (!this->receiveBytes())
    {

        ESP_LOGE("DALY-BMS DEBUG", "Receive failed, Temp values won't be modified!");

        return false;
    }

    // An offset of 40 is added by the BMS to avoid having to deal with negative numbers, see protocol in /docs/
    get.tempMax = (this->my_rxBuffer[4] - 40);
    get.tempMin = (this->my_rxBuffer[6] - 40);
    get.tempAverage = (get.tempMax + get.tempMin) / 2;

    return true;
}

bool Daly_BMS_UART::getDischargeChargeMosStatus() // 0x93
{
    this->sendCommand(COMMAND::DISCHARGE_CHARGE_MOS_STATUS);

    if (!this->receiveBytes())
    {

        ESP_LOGE("DALY-BMS DEBUG", "Receive failed, Charge / discharge mos Status won't be modified!");

        return false;
    }

    switch (this->my_rxBuffer[4])
    {
    case 0:
        get.chargeDischargeStatus = "Stationary";
        break;
    case 1:
        get.chargeDischargeStatus = "Charge";
        break;
    case 2:
        get.chargeDischargeStatus = "Discharge";
        break;
    }

    get.chargeFetState = this->my_rxBuffer[5];
    get.disChargeFetState = this->my_rxBuffer[6];
    get.bmsHeartBeat = this->my_rxBuffer[7];
    get.resCapacitymAh = ((uint32_t)my_rxBuffer[8] << 0x18) | ((uint32_t)my_rxBuffer[9] << 0x10) | ((uint32_t)my_rxBuffer[10] << 0x08) | (uint32_t)my_rxBuffer[11];

    return true;
}

bool Daly_BMS_UART::getStatusInfo() // 0x94
{
    this->sendCommand(COMMAND::STATUS_INFO);

    if (!this->receiveBytes())
    {

        ESP_LOGE("DALY-BMS DEBUG", "Receive failed, Status info won't be modified!");

        return false;
    }

    get.numberOfCells = this->my_rxBuffer[4];
    get.numOfTempSensors = this->my_rxBuffer[5];
    get.chargeState = this->my_rxBuffer[6];
    get.loadState = this->my_rxBuffer[7];

    // Parse the 8 bits into 8 booleans that represent the states of the Digital IO
    for (size_t i = 0; i < 8; i++)
    {
        get.dIO[i] = (this->my_rxBuffer[8] >> i) & 0x01;
    }

    get.bmsCycles = ((uint16_t)this->my_rxBuffer[9] << 0x08) | (uint16_t)this->my_rxBuffer[10];

    return true;
}

bool Daly_BMS_UART::getCellVoltages() // 0x95
{
    int cellNo = 0;

    // Check to make sure we have a valid number of cells
    if (get.numberOfCells < MIN_NUMBER_CELLS && get.numberOfCells >= MAX_NUMBER_CELLS)
    {
        return false;
    }

    this->sendCommand(COMMAND::CELL_VOLTAGES);

    for (size_t i = 0; i <= get.numberOfCells / 3; i++)
    {
        if (!this->receiveBytes())
        {
            ESP_LOGE("DALY-BMS DEBUG", "Receive failed, Cell Voltages won't be modified!");
            return false;
        }

        for (size_t i = 0; i < 3; i++)
        {

            ESP_LOGD("DALY-BMS DEBUG", "Frame No.: %d Cell No: %d. %dmV", this->my_rxBuffer[4], cellNo + 1, ((this->my_rxBuffer[5 + i + i] << 8) | this->my_rxBuffer[6 + i + i]));
            get.cellVmV[cellNo] = (this->my_rxBuffer[5 + i + i] << 8) | this->my_rxBuffer[6 + i + i];
            cellNo++;
            if (cellNo >= get.numberOfCells)
                break;
        }
    }

    return true;
}

bool Daly_BMS_UART::getCellTemperature() // 0x96
{
    int sensorNo = 0;

    // Check to make sure we have a valid number of temp sensors
    if ((get.numOfTempSensors < MIN_NUMBER_TEMP_SENSORS) && (get.numOfTempSensors >= MAX_NUMBER_TEMP_SENSORS))
    {
        return false;
    }

    this->sendCommand(COMMAND::CELL_TEMPERATURE);

    for (size_t i = 0; i <= get.numOfTempSensors / 7; i++)
    {

        if (!this->receiveBytes())
        {

            ESP_LOGE("DALY-BMS DEBUG", "Receive failed, Cell Temperatures won't be modified!");

            return false;
        }

        for (size_t i = 0; i < 7; i++)
        {


            ESP_LOGD("DALY-BMS DEBUG", "Frame No.: %d Sensor No: %d. %d°C", this->my_rxBuffer[4], sensorNo + 1, this->my_rxBuffer[5 + i] - 40);

            get.cellTemperature[sensorNo] = (this->my_rxBuffer[5 + i] - 40);
            sensorNo++;
            if (sensorNo + 1 >= get.numOfTempSensors)
                break;
        }
    }
    return true;
}

bool Daly_BMS_UART::getCellBalanceState() // 0x97
{
    int cellBalance = 0;
    int cellBit = 0;

    // Check to make sure we have a valid number of cells
    if (get.numberOfCells < MIN_NUMBER_CELLS && get.numberOfCells >= MAX_NUMBER_CELLS)
    {
        return false;
    }

    this->sendCommand(COMMAND::CELL_BALANCE_STATE);

    if (!this->receiveBytes())
    {

        ESP_LOGE("DALY-BMS DEBUG", "Receive failed, Cell Balance State won't be modified!");

        return false;
    }

    // We expect 6 bytes response for this command
    for (size_t i = 0; i < 6; i++)
    {
        // For each bit in the byte, pull out the cell balance state boolean
        for (size_t j = 0; j < 8; j++)
        {
            get.cellBalanceState[cellBit] = (this->my_rxBuffer[i + 4] >> j) & 0x01;
            cellBit++;
            if ((this->my_rxBuffer[i + 4] >> j) & 0x01)
            {
                cellBalance++;
            }
            if (cellBit >= 47)
            {
                break;
            }
        }
    }


    ESP_LOGD("DALY-BMS DEBUG", "Cell Balance State: ");
    for (int i = 0; i < get.numberOfCells; i++)
    {
        ESP_LOGD("DALY-BMS DEBUG", "%d", get.cellBalanceState[i]);
    }


    if (cellBalance > 0)
    {
        get.cellBalanceActive = true;
    }
    else
    {
        get.cellBalanceActive = false;
    }

    return true;
}

bool Daly_BMS_UART::getFailureCodes() // 0x98
{
    this->sendCommand(COMMAND::FAILURE_CODES);

    if (!this->receiveBytes())
    {

        ESP_LOGE("DALY-BMS DEBUG", "Receive failed, Failure Flags won't be modified!");

        return false;
    }

    /* 0x00 */
    alarm.levelOneCellVoltageTooHigh = (this->my_rxBuffer[4] >> 0) & 0x01;
    alarm.levelTwoCellVoltageTooHigh = (this->my_rxBuffer[4] >> 1) & 0x01;
    alarm.levelOneCellVoltageTooLow = (this->my_rxBuffer[4] >> 2) & 0x01;
    alarm.levelTwoCellVoltageTooLow = (this->my_rxBuffer[4] >> 3) & 0x01;
    alarm.levelOnePackVoltageTooHigh = (this->my_rxBuffer[4] >> 4) & 0x01;
    alarm.levelTwoPackVoltageTooHigh = (this->my_rxBuffer[4] >> 5) & 0x01;
    alarm.levelOnePackVoltageTooLow = (this->my_rxBuffer[4] >> 6) & 0x01;
    alarm.levelTwoPackVoltageTooLow = (this->my_rxBuffer[4] >> 7) & 0x01;

    /* 0x01 */
    alarm.levelOneChargeTempTooHigh = (this->my_rxBuffer[5] >> 1) & 0x01;
    alarm.levelTwoChargeTempTooHigh = (this->my_rxBuffer[5] >> 1) & 0x01;
    alarm.levelOneChargeTempTooLow = (this->my_rxBuffer[5] >> 1) & 0x01;
    alarm.levelTwoChargeTempTooLow = (this->my_rxBuffer[5] >> 1) & 0x01;
    alarm.levelOneDischargeTempTooHigh = (this->my_rxBuffer[5] >> 1) & 0x01;
    alarm.levelTwoDischargeTempTooHigh = (this->my_rxBuffer[5] >> 1) & 0x01;
    alarm.levelOneDischargeTempTooLow = (this->my_rxBuffer[5] >> 1) & 0x01;
    alarm.levelTwoDischargeTempTooLow = (this->my_rxBuffer[5] >> 1) & 0x01;

    /* 0x02 */
    alarm.levelOneChargeCurrentTooHigh = (this->my_rxBuffer[6] >> 0) & 0x01;
    alarm.levelTwoChargeCurrentTooHigh = (this->my_rxBuffer[6] >> 1) & 0x01;
    alarm.levelOneDischargeCurrentTooHigh = (this->my_rxBuffer[6] >> 2) & 0x01;
    alarm.levelTwoDischargeCurrentTooHigh = (this->my_rxBuffer[6] >> 3) & 0x01;
    alarm.levelOneStateOfChargeTooHigh = (this->my_rxBuffer[6] >> 4) & 0x01;
    alarm.levelTwoStateOfChargeTooHigh = (this->my_rxBuffer[6] >> 5) & 0x01;
    alarm.levelOneStateOfChargeTooLow = (this->my_rxBuffer[6] >> 6) & 0x01;
    alarm.levelTwoStateOfChargeTooLow = (this->my_rxBuffer[6] >> 7) & 0x01;

    /* 0x03 */
    alarm.levelOneCellVoltageDifferenceTooHigh = (this->my_rxBuffer[7] >> 0) & 0x01;
    alarm.levelTwoCellVoltageDifferenceTooHigh = (this->my_rxBuffer[7] >> 1) & 0x01;
    alarm.levelOneTempSensorDifferenceTooHigh = (this->my_rxBuffer[7] >> 2) & 0x01;
    alarm.levelTwoTempSensorDifferenceTooHigh = (this->my_rxBuffer[7] >> 3) & 0x01;

    /* 0x04 */
    alarm.chargeFETTemperatureTooHigh = (this->my_rxBuffer[8] >> 0) & 0x01;
    alarm.dischargeFETTemperatureTooHigh = (this->my_rxBuffer[8] >> 1) & 0x01;
    alarm.failureOfChargeFETTemperatureSensor = (this->my_rxBuffer[8] >> 2) & 0x01;
    alarm.failureOfDischargeFETTemperatureSensor = (this->my_rxBuffer[8] >> 3) & 0x01;
    alarm.failureOfChargeFETAdhesion = (this->my_rxBuffer[8] >> 4) & 0x01;
    alarm.failureOfDischargeFETAdhesion = (this->my_rxBuffer[8] >> 5) & 0x01;
    alarm.failureOfChargeFETTBreaker = (this->my_rxBuffer[8] >> 6) & 0x01;
    alarm.failureOfDischargeFETBreaker = (this->my_rxBuffer[8] >> 7) & 0x01;

    /* 0x05 */
    alarm.failureOfAFEAcquisitionModule = (this->my_rxBuffer[9] >> 0) & 0x01;
    alarm.failureOfVoltageSensorModule = (this->my_rxBuffer[9] >> 1) & 0x01;
    alarm.failureOfTemperatureSensorModule = (this->my_rxBuffer[9] >> 2) & 0x01;
    alarm.failureOfEEPROMStorageModule = (this->my_rxBuffer[9] >> 3) & 0x01;
    alarm.failureOfRealtimeClockModule = (this->my_rxBuffer[9] >> 4) & 0x01;
    alarm.failureOfPrechargeModule = (this->my_rxBuffer[9] >> 5) & 0x01;
    alarm.failureOfVehicleCommunicationModule = (this->my_rxBuffer[9] >> 6) & 0x01;
    alarm.failureOfIntranetCommunicationModule = (this->my_rxBuffer[9] >> 7) & 0x01;

    /* 0x06 */
    alarm.failureOfCurrentSensorModule = (this->my_rxBuffer[10] >> 0) & 0x01;
    alarm.failureOfMainVoltageSensorModule = (this->my_rxBuffer[10] >> 1) & 0x01;
    alarm.failureOfShortCircuitProtection = (this->my_rxBuffer[10] >> 2) & 0x01;
    alarm.failureOfLowVoltageNoCharging = (this->my_rxBuffer[10] >> 3) & 0x01;

    return true;
}

bool Daly_BMS_UART::setDischargeMOS(bool sw) // 0xD9 0x80 First Byte 0x01=ON 0x00=OFF
{
    if (sw)
    {

        ESP_LOGD("DALY-BMS DEBUG", "Attempting to switch discharge MOSFETs on");

        // Set the first byte of the data payload to 1, indicating that we want to switch on the MOSFET
        this->my_txBuffer[4] = 0x01;
        this->sendCommand(COMMAND::DISCHRG_FET);
        // Clear the buffer for further use
        this->my_txBuffer[4] = 0x00;
    }
    else
    {

        ESP_LOGD("DALY-BMS DEBUG", "Attempting to switch discharge MOSFETs off");

        this->sendCommand(COMMAND::DISCHRG_FET);
    }
    if (!this->receiveBytes())
    {

        ESP_LOGE("DALY-BMS DEBUG", "No response from BMS! Can't verify MOSFETs switched.");

        return false;
    }

    return true;
}

bool Daly_BMS_UART::setChargeMOS(bool sw) // 0xDA 0x80 First Byte 0x01=ON 0x00=OFF
{
    if (sw == true)
    {

        ESP_LOGD("DALY-BMS DEBUG", "Attempting to switch charge MOSFETs on");

        // Set the first byte of the data payload to 1, indicating that we want to switch on the MOSFET
        this->my_txBuffer[4] = 0x01;
        this->sendCommand(COMMAND::CHRG_FET);
        // Clear the buffer for further use
        this->my_txBuffer[4] = 0x00;
    }
    else
    {

        ESP_LOGD("DALY-BMS DEBUG", "Attempting to switch charge MOSFETs off");

        this->sendCommand(COMMAND::CHRG_FET);
    }

    if (!this->receiveBytes())
    {

        ESP_LOGE("DALY-BMS DEBUG", "No response from BMS! Can't verify MOSFETs switched.");

        return false;
    }

    return true;
}

bool Daly_BMS_UART::setBmsReset() // 0x00 Reset the BMS
{
    this->sendCommand(COMMAND::BMS_RESET);

    if (!this->receiveBytes())
    {

        ESP_LOGE("DALY-BMS DEBUG", "Send failed, can't verify BMS was reset!");

        return false;
    }

    return true;
}

//----------------------------------------------------------------------
// Private Functions
//----------------------------------------------------------------------

void Daly_BMS_UART::sendCommand(COMMAND cmdID)
{
    // Clear all incoming UART to avoid data collision
    while (uart_read_bytes(this->uart_port, this->my_rxBuffer, 1, 10 / portTICK_RATE_MS) > 0);

    uint8_t checksum = 0;
    this->my_txBuffer[2] = cmdID;
    // Calculate the checksum
    for (uint8_t i = 0; i <= 11; i++)
    {
        checksum += this->my_txBuffer[i];
    }
    // put it on the frame
    this->my_txBuffer[12] = checksum;

    // ESP_LOGD("DALY-BMS DEBUG", "Send command: 0x%x Checksum = 0x%x", cmdID, checksum);

    uart_write_bytes(this->uart_port, (const char *)this->my_txBuffer, XFER_BUFFER_LENGTH);
}

bool Daly_BMS_UART::receiveBytes(void)
{
    // Clear out the input buffer
    memset(this->my_rxBuffer, 0, XFER_BUFFER_LENGTH);

    // Read bytes from the specified UART interface
    int rxByteNum = uart_read_bytes(this->uart_port, this->my_rxBuffer, XFER_BUFFER_LENGTH, 1000 / portTICK_RATE_MS);

    // Make sure we got the correct number of bytes
    if (rxByteNum != XFER_BUFFER_LENGTH)
    {

        ESP_LOGE("DALY-BMS DEBUG", "Error: Received the wrong number of bytes! Expected 13, got %d", rxByteNum);
        this->barfRXBuffer();

        return false;
    }

    if (!validateChecksum())
    {

        ESP_LOGE("DALY-BMS DEBUG", "Error: Checksum failed!");
        this->barfRXBuffer();

        return false;
    }
    return true;
}

bool Daly_BMS_UART::validateChecksum()
{
    uint8_t checksum = 0x00;

    for (int i = 0; i < XFER_BUFFER_LENGTH - 1; i++)
    {
        checksum += this->my_rxBuffer[i];
    }

    // ESP_LOGD("DALY-BMS DEBUG", "Calculated checksum: %d, Received: %d", checksum, this->my_rxBuffer[XFER_BUFFER_LENGTH - 1]);

    // Compare the calculated checksum to the real checksum (the last received byte)
    return (checksum == this->my_rxBuffer[XFER_BUFFER_LENGTH - 1]);
}

void Daly_BMS_UART::barfRXBuffer(void)
{

    ESP_LOGD("DALY-BMS DEBUG", "RX Buffer: [");
    for (int i = 0; i < XFER_BUFFER_LENGTH; i++)
    {
        ESP_LOGD("DALY-BMS DEBUG", ",0x%x", this->my_rxBuffer[i]);
    }
    ESP_LOGD("DALY-BMS DEBUG", "]");

}
