#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"
#include <stdint.h>

#define MAX_CELLS 14

typedef struct {
    float voltage_V_f;
    float current_A_f;
    uint8_t soh_perc_ui8;
    uint8_t dalySoc_perc_ui8;
    uint8_t ourSoc_perc_ui8;
    uint16_t dalyCapacity_Ah_ui16;
    float ourInternalResistance_mOhm_f;
} pack_data_t;

typedef struct {
    float voltage;
    float soc;
} cell_data_t;

void ui_manager_init(void);

// Pack data getters and setters
void ui_manager_set_pack_data(pack_data_t data);
pack_data_t ui_manager_get_pack_data(void);

// Individual setters for pack data
void ui_manager_set_pack_voltage_V(float voltage);
void ui_manager_set_pack_current_A(float current);
void ui_manager_set_pack_soh_perc(uint8_t soh);
void ui_manager_set_daly_soc_perc(uint8_t soc);
void ui_manager_set_our_soc_perc(uint8_t soc);
void ui_manager_set_daly_capacity_Ah(uint16_t capacity);
void ui_manager_set_our_internal_resistance_mOhm(float resistance);

// Cell data getters and setters
void ui_manager_set_cell_data(int cell_index, cell_data_t data);
cell_data_t ui_manager_get_cell_data(int cell_index);

// Individual setters for cell data
void ui_manager_set_cell_voltage(int cell_index, float voltage);
void ui_manager_set_cell_soc(int cell_index, float soc);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // UI_MANAGER_H