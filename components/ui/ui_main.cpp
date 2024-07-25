#include "ui_main.h"
#include "esp_log.h"

static const char* TAG = "UIMain";

UIMain::UIMain() : menu(nullptr), packDataPage(nullptr), cellDataPage(nullptr), settingsPage(nullptr) {
    ESP_LOGI(TAG, "UIMain constructor called");
}

UIMain::~UIMain() {
    // Cleanup if necessary
}

bool UIMain::init() {
    ESP_LOGI(TAG, "Initializing UIMain");
    createMenu();
    return true;
}

void UIMain::show() {
    if (menu == nullptr) {
        ESP_LOGE(TAG, "Menu not initialized. Call init() first.");
        return;
    }
    lv_scr_load(menu);
}

UIManager& UIMain::getUIManager() {
    return uiManager;
}

void UIMain::createMenu() {
    menu = lv_menu_create(lv_screen_active());
    lv_obj_set_size(menu, lv_display_get_horizontal_resolution(NULL), lv_display_get_vertical_resolution(NULL));
    lv_obj_center(menu);

    lv_menu_set_mode_root_back_button(menu, LV_MENU_ROOT_BACK_BUTTON_ENABLED);
    lv_obj_add_event_cb(menu, menuBackEventHandler, LV_EVENT_CLICKED, menu);

    createPackDataPage();
    createCellDataPage();
    createSettingsPage();

    // Create root page
    lv_obj_t* root_page = lv_menu_page_create(menu, "Main Menu");
    lv_obj_t* section = lv_menu_section_create(root_page);

    const char* menu_items[] = {"Pack Data", "Individual Cell Data", "Settings"};
    lv_obj_t* pages[] = {packDataPage, cellDataPage, settingsPage};

    for (int i = 0; i < 3; i++) {
        lv_obj_t* cont = lv_menu_cont_create(section);
        lv_obj_t* label = lv_label_create(cont);
        lv_label_set_text(label, menu_items[i]);
        lv_menu_set_load_page_event(menu, cont, pages[i]);

        // Add click event to each menu item
        lv_obj_add_event_cb(cont, [](lv_event_t* e) {
            const char* item_name = (const char*)lv_event_get_user_data(e);
            ESP_LOGI(TAG, "Clicked on menu item: %s", item_name);
        }, LV_EVENT_CLICKED, (void*)menu_items[i]);

        // Log position and size of each menu item
        int16_t x = (int16_t)lv_obj_get_x(cont);
        int16_t y = (int16_t)lv_obj_get_y(cont);
        int16_t w = (int16_t)lv_obj_get_width(cont);
        int16_t h = (int16_t)lv_obj_get_height(cont);
        ESP_LOGD(TAG, "Menu item '%s': Top-left (x: %d, y: %d), Bottom-right (x: %d, y: %d)", 
                 menu_items[i], x, y, x + w, y + h);
    }

    lv_menu_set_page(menu, root_page);

    // Add event to log when a page is loaded
    // lv_obj_add_event_cb(menu, [](lv_event_t* e) {
    //     lv_obj_t* page = lv_menu_get_cur_main_page((lv_obj_t*)lv_event_get_target(e));
    //     if (page) {
    //         const char* title = lv_menu_get_page(page);
    //         ESP_LOGI(TAG, "Loaded page: %s", title ? title : "Untitled");
    //     }
    // }, LV_EVENT_VALUE_CHANGED, NULL);
}

void UIMain::createPackDataPage() {
    packDataPage = lv_menu_page_create(menu, "Pack Data");
    lv_obj_t* cont = lv_menu_cont_create(packDataPage);
    lv_obj_t* label = lv_label_create(cont);
    lv_label_set_text(label, "Pack Data Content");
    
    // Add a colored rectangle to make the page visually distinct
    lv_obj_t* rect = lv_obj_create(cont);
    lv_obj_set_size(rect, 100, 50);
    lv_obj_set_style_bg_color(rect, lv_color_hex(0xFF0000), 0);  // Red
    lv_obj_center(rect);
}

void UIMain::createCellDataPage() {
    cellDataPage = lv_menu_page_create(menu, "Individual Cell Data");
    lv_obj_t* cont = lv_menu_cont_create(cellDataPage);
    lv_obj_t* label = lv_label_create(cont);
    lv_label_set_text(label, "Cell Data Content");
    
    // Add a colored rectangle to make the page visually distinct
    lv_obj_t* rect = lv_obj_create(cont);
    lv_obj_set_size(rect, 100, 50);
    lv_obj_set_style_bg_color(rect, lv_color_hex(0x00FF00), 0);  // Green
    lv_obj_center(rect);
}

void UIMain::createSettingsPage() {
    settingsPage = lv_menu_page_create(menu, "Settings");
    lv_obj_t* cont = lv_menu_cont_create(settingsPage);
    lv_obj_t* label = lv_label_create(cont);
    lv_label_set_text(label, "Settings Content");
    
    // Add a colored rectangle to make the page visually distinct
    lv_obj_t* rect = lv_obj_create(cont);
    lv_obj_set_size(rect, 100, 50);
    lv_obj_set_style_bg_color(rect, lv_color_hex(0x0000FF), 0);  // Blue
    lv_obj_center(rect);
}

void UIMain::menuBackEventHandler(lv_event_t* e) {
    lv_obj_t* obj = (lv_obj_t*)lv_event_get_target(e);
    lv_obj_t* menu = (lv_obj_t*)lv_event_get_user_data(e);

    if (lv_menu_back_button_is_root(menu, obj)) {
        ESP_LOGI(TAG, "Root back button clicked");
    }
}