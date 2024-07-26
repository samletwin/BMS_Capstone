#include "ui_manager.h"
#include "esp_log.h"
#include <string.h>

static const char* TAG = "UI_MANAGER";

static pack_data_t pack_data;
static cell_data_t cell_data[MAX_CELLS];

void ui_manager_init(void)
{
    ESP_LOGI(TAG, "Initializing UI Manager");
    memset(&pack_data, 0, sizeof(pack_data_t));
    memset(cell_data, 0, sizeof(cell_data_t) * MAX_CELLS);
}

void ui_manager_set_pack_data(pack_data_t *data)
{
    if (data) {
        memcpy(&pack_data, data, sizeof(pack_data_t));
    }
}

pack_data_t ui_manager_get_pack_data(void)
{
    return pack_data;
}

void ui_manager_set_pack_voltage_V(float *voltage)
{
    if (voltage) {
        pack_data.voltage_V_f = *voltage;
    }
}

void ui_manager_set_pack_current_A(float *current)
{
    if (current) {
        pack_data.current_A_f = *current;
    }
}

void ui_manager_set_pack_soh_perc(uint8_t *soh)
{
    if (soh) {
        pack_data.soh_perc_ui8 = *soh;
    }
}

void ui_manager_set_daly_soc_perc(float *soc)
{
    if (soc) {
        pack_data.dalySoc_perc_f = *soc;
    }
}

void ui_manager_set_our_soc_perc(float *soc)
{
    if (soc) {
        pack_data.ourSoc_perc_f = *soc;
    }
}

void ui_manager_set_daly_capacity_Ah(uint16_t *capacity)
{
    if (capacity) {
        pack_data.dalyCapacity_Ah_ui16 = *capacity;
    }
}

void ui_manager_set_our_internal_resistance_mOhm(float *resistance)
{
    if (resistance) {
        pack_data.ourInternalResistance_mOhm_f = *resistance;
    }
}

void ui_manager_set_cell_data(int cell_index, cell_data_t *data)
{
    if (cell_index >= 0 && cell_index < MAX_CELLS && data) {
        memcpy(&cell_data[cell_index], data, sizeof(cell_data_t));
    }
}

cell_data_t ui_manager_get_cell_data(int cell_index)
{
    if (cell_index >= 0 && cell_index < MAX_CELLS) {
        return cell_data[cell_index];
    }
    cell_data_t empty = {0};
    return empty;
}

void ui_manager_set_cell_voltage(int cell_index, float *voltage)
{
    if (cell_index >= 0 && cell_index < MAX_CELLS && voltage) {
        cell_data[cell_index].voltage = *voltage;
    }
}

void ui_manager_set_cell_soc(int cell_index, float *soc)
{
    if (cell_index >= 0 && cell_index < MAX_CELLS && soc) {
        cell_data[cell_index].soc = *soc;
    }
}

void ui_manager_set_all_cell_voltages(float *voltages)
{
    if (voltages) {
        for (int i = 0; i < MAX_CELLS; i++) {
            cell_data[i].voltage = (voltages[i]) /1000;
        }
    }
}

void ui_manager_set_all_cell_socs(float *socs)
{
    if (socs) {
        for (int i = 0; i < MAX_CELLS; i++) {
            cell_data[i].soc = socs[i];
        }
    }
}