#include "ui_screen.h"
#include "esp_log.h"
#include "battery_widget.h"
#include "stdio.h"
#include "colour_swap.h"
#include "ui_manager.h"

static const char* TAG = "UI_SCREEN";

static lv_obj_t* menu;
static lv_obj_t* packDataPage;
static lv_obj_t* cellDataPage;
static lv_obj_t* settingsPage;

static battery_widget_t *daly_bms_soc_bar = NULL;
static battery_widget_t *our_soc_bar = NULL;
static lv_obj_t* packDataTable = NULL;
static lv_obj_t* currentLoadedSubPage = NULL;
static bool show_voltage = true;  // Global variable to track what data to show

static void cell_data_switch_cb(lv_event_t * e);

// Add these function declarations at the top of the file
static void tick_pack_data_screen(lv_obj_t *parent);
static void tick_cell_data_screen(lv_obj_t *parent);

// Implement the functions

static void tick_pack_data_screen(lv_obj_t *parent)
{
    pack_data_t pack_data = ui_manager_get_pack_data();

    pack_data.dalySoc_perc_f = 83.0f;
    pack_data.ourSoc_perc_f = 9.0f;
    // Update Daly BMS SOC battery
    if (daly_bms_soc_bar != NULL) {
        battery_widget_set_value(daly_bms_soc_bar, (int32_t)pack_data.dalySoc_perc_f);
    }
    else {
        ESP_LOGE(TAG, "Unable to update daly bms soc bar because it is null");
    }

    if (our_soc_bar != NULL) {
        battery_widget_set_value(our_soc_bar, (int32_t)pack_data.ourSoc_perc_f);
    }
    else {
        ESP_LOGE(TAG, "Unable to update our bms soc bar because it is null");
    }

    // Update table values
    // lv_obj_t *packDataTable = lv_obj_get_child(parent, 2);
    char buf[20];
    // pack_data.voltage_V_f = 50.0f; 
    if (packDataTable != NULL){
    snprintf(buf, sizeof(buf), "%.2fV", pack_data.voltage_V_f);
    lv_table_set_cell_value(packDataTable, 0, 1, buf);
    snprintf(buf, sizeof(buf), "%uAh", pack_data.dalyCapacity_Ah_ui16);
    lv_table_set_cell_value(packDataTable, 1, 1, buf);
    snprintf(buf, sizeof(buf), "%.2fA", pack_data.current_A_f);
    lv_table_set_cell_value(packDataTable, 2, 1, buf);
    snprintf(buf, sizeof(buf), "%.3f mOhm", pack_data.ourInternalResistance_mOhm_f);
    lv_table_set_cell_value(packDataTable, 3, 1, buf);
    }
    else
    {
        ESP_LOGE(TAG, "Unable to update our pack table because it is null");

    }
}

static void tick_cell_data_screen(lv_obj_t *parent)
{
    lv_obj_t *cont = lv_obj_get_child(parent, 0);

    for (int i = 0; i < 14; i++) {
        lv_obj_t *cell = lv_obj_get_child(cont, i);
        lv_obj_t *label = lv_obj_get_child(cell, 0);

        cell_data_t cell_data = ui_manager_get_cell_data(i);

        char buf[30];
        if (show_voltage) {
            snprintf(buf, sizeof(buf), "Cell %d\n%.3fV", i + 1, cell_data.voltage);
        } else {
            snprintf(buf, sizeof(buf), "Cell %d\n%.1f%%", i + 1, cell_data.soc);
        }
        lv_label_set_text(label, buf);
    }
}

void ui_screen_tick_active(void)
{
    lv_obj_t *active_page = lv_menu_get_cur_main_page(menu);
    
    if (active_page == packDataPage) {
        tick_pack_data_screen(active_page);
    } else if (active_page == cellDataPage) {
        tick_cell_data_screen(active_page);
    }
    // Add more conditions for other pages if needed
}

static void menuBackEventHandler(lv_event_t* e) {
    lv_obj_t* obj = lv_event_get_target(e);
    lv_obj_t* menu = lv_event_get_user_data(e);
    if (lv_menu_back_button_is_root(menu, obj)) {
        ESP_LOGI(TAG, "Root back button clicked");
    }
}

static void menuValueChangedEventHandler(lv_event_t* e) {
    lv_obj_t* menu = lv_event_get_target(e);
    lv_obj_t* main_page = lv_event_get_user_data(e);
    lv_obj_t* page = lv_menu_get_cur_main_page(menu);
    
    /* We have returned from a sub page to the main page - delete the objects of the sub page*/
    if (main_page == page) {
        // delete_sub_page_screen(currentLoadedSubPage);
        if (currentLoadedSubPage != NULL) {
            ESP_LOGI(TAG, "Cleaning sub page");
            lv_obj_clean(currentLoadedSubPage);
        }
        currentLoadedSubPage = NULL;
    }
    else {
        currentLoadedSubPage = page;
    }
}

static void create_pack_data_screen(lv_obj_t *parent)
{
    #define BATT_CONTAINER_WIDTH_PERC 45
    #define TABLE_WIDTH_PERC 55
    // Create a flex container for the whole screen
    lv_obj_t *main_cont = lv_obj_create(parent);
    lv_obj_set_size(main_cont, lv_pct(100), lv_pct(100));
    lv_obj_set_layout(main_cont, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(main_cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_all(main_cont, 0, 0);  // Set padding to zero
    lv_obj_set_style_border_width(main_cont, 0, 0);  // Remove border

    // Create a container for batteries (without visible boundaries and scrollbars)
    lv_obj_t *battery_cont = lv_obj_create(main_cont);
    lv_obj_set_size(battery_cont, lv_pct(BATT_CONTAINER_WIDTH_PERC), lv_pct(100));
    lv_obj_set_layout(battery_cont, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(battery_cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(battery_cont, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_border_width(battery_cont, 0, 0);  // Remove border
    lv_obj_set_style_bg_opa(battery_cont, LV_OPA_TRANSP, 0);  // Make background transparent
    lv_obj_clear_flag(battery_cont, LV_OBJ_FLAG_SCROLLABLE);  // Remove scrollbars

    pack_data_t pack_data = ui_manager_get_pack_data();

    // Create Daly BMS SOC battery
    daly_bms_soc_bar = battery_widget_create(battery_cont, false, "Daly BMS SOC");
    lv_obj_set_size(daly_bms_soc_bar->bar, lv_pct(80), 50);
    battery_widget_set_value(daly_bms_soc_bar, (uint16_t)pack_data.dalySoc_perc_f);

    // Create Our SOC battery
    our_soc_bar = battery_widget_create(battery_cont, false, "Our SOC");
    lv_obj_set_size(our_soc_bar->bar, lv_pct(80), 50);
    battery_widget_set_value(our_soc_bar, (uint16_t)pack_data.ourSoc_perc_f);


    // Create table
    packDataTable = lv_table_create(main_cont);
    lv_obj_set_size(packDataTable, lv_pct(TABLE_WIDTH_PERC), lv_pct(100));
    
    lv_table_set_column_count(packDataTable, 2);
    lv_table_set_row_count(packDataTable, 4);
   
    lv_table_set_cell_value(packDataTable, 0, 0, "Voltage");
    lv_table_set_cell_value(packDataTable, 1, 0, "Capacity");
    lv_table_set_cell_value(packDataTable, 2, 0, "Current");
    lv_table_set_cell_value(packDataTable, 3, 0, "Resistance");

    // Set values from pack_data
    char buf[20];
    snprintf(buf, sizeof(buf), "%.2fV", pack_data.voltage_V_f);
    lv_table_set_cell_value(packDataTable, 0, 1, buf);
    snprintf(buf, sizeof(buf), "%uAh", pack_data.dalyCapacity_Ah_ui16);
    lv_table_set_cell_value(packDataTable, 1, 1, buf);
    snprintf(buf, sizeof(buf), "%.2fA", pack_data.current_A_f);
    lv_table_set_cell_value(packDataTable, 2, 1, buf);
    snprintf(buf, sizeof(buf), "%.3f mOhm", pack_data.ourInternalResistance_mOhm_f);
    lv_table_set_cell_value(packDataTable, 3, 1, buf);

    // Set some initial values 
    // lv_table_set_cell_value(table, 0, 1, "48.2V");
    // lv_table_set_cell_value(table, 1, 1, "95%");
    // lv_table_set_cell_value(table, 2, 1, "100Ah");
    // lv_table_set_cell_value(table, 3, 1, "2.5A");
    // lv_table_set_cell_value(table, 4, 1, "0.1 Ohm");

    // lv_obj_set_style_border_side(table, LV_BORDER_SIDE_FULL, LV_PART_ITEMS);
    // Remove table padding
    lv_obj_set_style_pad_ver(packDataTable, 25, LV_PART_ITEMS);
    lv_obj_set_style_pad_hor(packDataTable, 30, LV_PART_ITEMS);
    lv_obj_clear_flag(packDataTable, LV_OBJ_FLAG_CLICKABLE);

    // Adjust table column widths
    uint16_t disp_width = lv_display_get_horizontal_resolution(NULL);
    uint16_t col_width = disp_width*TABLE_WIDTH_PERC / 100;
    lv_table_set_col_width(packDataTable, 0, col_width*0.55);
    lv_table_set_col_width(packDataTable, 1, col_width*0.45);

    // lv_obj_set_style_bg_opa(table, LV_OPA_TRANSP, 0);  // Make background transparent
    lv_obj_set_scroll_dir(packDataTable, LV_DIR_VER);

    // Reduce row length via padding
    // lv_obj_set_style_pad_ver(table, 40, LV_PART_ITEMS);

    // Set some initial battery values
    battery_widget_set_value(daly_bms_soc_bar, 75);
    battery_widget_set_value(our_soc_bar, 80);

    // Adjust layout
    lv_obj_set_flex_align(main_cont, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
}


static void create_cell_data_screen(lv_obj_t *parent)
{
    // Clear the previous content
    lv_obj_clean(parent);

    // Create a container for the grid
    lv_obj_t *cont = lv_obj_create(parent);
    lv_obj_set_size(cont, lv_pct(100), lv_pct(95));  // Leave some space for the button
    lv_obj_set_style_pad_all(cont, 0, 0);

    // Create grid descriptor with 5 columns
    static lv_coord_t col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static lv_coord_t row_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};

    lv_obj_set_grid_dsc_array(cont, col_dsc, row_dsc);

    // Reduce padding between grid items
    lv_obj_set_style_pad_row(cont, 1, 0);
    lv_obj_set_style_pad_column(cont, 1, 0);

    // Create cell objects
    for (int i = 0; i < 14; i++) {
        lv_obj_t *cell = lv_obj_create(cont);
        lv_obj_set_size(cell, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        lv_obj_clear_flag(cell, LV_OBJ_FLAG_SCROLLABLE);

        cell_data_t cell_data = ui_manager_get_cell_data(i);

        // Create label for cell
        lv_obj_t *label = lv_label_create(cell);
        char buf[30];
        if (show_voltage) {
            snprintf(buf, sizeof(buf), "Cell %d\n%.3fV", i + 1, cell_data.voltage);
        } else {
            snprintf(buf, sizeof(buf), "Cell %d\n%.1f%%", i + 1, cell_data.soc);
        }
        lv_label_set_text(label, buf);
        lv_obj_set_style_text_font(label, &lv_font_montserrat_16, 0);
        lv_obj_center(label);

        // Set grid cell positions
        int row = i / 5;
        int col = i % 5;
        lv_obj_set_grid_cell(cell, LV_GRID_ALIGN_STRETCH, col, 1,
                             LV_GRID_ALIGN_STRETCH, row, 1);

        // Add border to cell
        lv_obj_set_style_border_width(cell, 1, 0);
        lv_obj_set_style_border_color(cell, lv_color_black(), 0);
    }

    // Create a button to switch between voltage and SOC
    lv_obj_t * switch_btn = lv_btn_create(parent);
    lv_obj_set_size(switch_btn, 100, 40);
    lv_obj_align(switch_btn, LV_ALIGN_BOTTOM_RIGHT, -10, -10);
    lv_obj_add_event_cb(switch_btn, cell_data_switch_cb, LV_EVENT_CLICKED, parent);

    lv_obj_t * btn_label = lv_label_create(switch_btn);
    lv_label_set_text(btn_label, show_voltage ? "Show SOC" : "Show Voltage");
    lv_obj_center(btn_label);
}

static void cell_data_switch_cb(lv_event_t * e)
{
    show_voltage = !show_voltage;
    lv_obj_t * parent = lv_event_get_user_data(e);
    create_cell_data_screen(parent);  // Recreate the screen with new data
}

static void settingsItemClickHandler(lv_event_t* e)
{
    ESP_LOGI(TAG, "Settings item clicked");
    lv_menu_set_page(menu, settingsPage);
}

static void cellDataItemClickHandler(lv_event_t* e)
{
    ESP_LOGI(TAG, "Individual Cell Data item clicked");
    create_cell_data_screen(cellDataPage);
    lv_menu_set_page(menu, cellDataPage);
}

static void packDataItemClickHandler(lv_event_t* e)
{
    ESP_LOGI(TAG, "Individual Cell Data item clicked");
    // Create screen
    create_pack_data_screen(packDataPage);
    lv_menu_set_page(menu, packDataPage);
}

void ui_screen_init(void)
{
    ESP_LOGI(TAG, "Initializing UI Screen");
}

lv_obj_t* ui_screen_get_menu(void)
{
    return menu;
}

void ui_screen_create_menu(void)
{
    menu = lv_menu_create(lv_screen_active());
    lv_obj_set_size(menu, lv_display_get_horizontal_resolution(NULL), lv_display_get_vertical_resolution(NULL));
    lv_obj_center(menu);
    lv_menu_set_mode_root_back_button(menu, LV_MENU_ROOT_BACK_BUTTON_ENABLED);
    lv_menu_set_mode_header(menu, LV_MENU_HEADER_TOP_FIXED);
    lv_obj_add_event_cb(menu, menuBackEventHandler, LV_EVENT_CLICKED, menu);

    // Create main page
    lv_obj_t* main_page = lv_menu_page_create(menu, "Main Menu");

    settingsPage = lv_menu_page_create(menu, "Settings");
    lv_obj_t* settingsCont = lv_menu_cont_create(settingsPage);
    lv_obj_t* settingsLabel = lv_label_create(settingsCont);
    lv_label_set_text(settingsLabel, "Settings Content");

    cellDataPage = lv_menu_page_create(menu, "Individual Cell Data");

    packDataPage = lv_menu_page_create(menu, "Battery Pack Data");

    // Create menu items in the main page
    lv_obj_t* cont;
    lv_obj_t* label;

    cont = lv_menu_cont_create(main_page);
    label = lv_label_create(cont);
    lv_label_set_text(label, "Pack Data");
    lv_menu_set_load_page_event(menu, cont, packDataPage);
    lv_obj_add_event_cb(cont, packDataItemClickHandler, LV_EVENT_CLICKED, NULL);

    cont = lv_menu_cont_create(main_page);
    label = lv_label_create(cont);
    lv_label_set_text(label, "Individual Cell Data");
    lv_menu_set_load_page_event(menu, cont, cellDataPage);
    lv_obj_add_event_cb(cont, cellDataItemClickHandler, LV_EVENT_CLICKED, NULL);

    cont = lv_menu_cont_create(main_page);
    label = lv_label_create(cont);
    lv_label_set_text(label, "Settings");
    lv_menu_set_load_page_event(menu, cont, settingsPage);
    lv_obj_add_event_cb(cont, settingsItemClickHandler, LV_EVENT_CLICKED, NULL);


    lv_obj_add_event_cb(menu, menuValueChangedEventHandler, LV_EVENT_VALUE_CHANGED, main_page);

    // Set the main page
    lv_menu_set_page(menu, main_page);
}