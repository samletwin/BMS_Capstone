#include "ui_main.h"
#include "ui_screen.h"
#include "ui_animation.h"
#include "ui_manager.h"
#include "esp_log.h"

static const char* TAG = "UI_MAIN";

void ui_main_init(void)
{
    ESP_LOGI(TAG, "Initializing UI Main");
    ui_screen_init();
    ui_animation_init();
    ui_manager_init();
    ui_screen_create_menu();
}

void ui_main_show(void)
{
    lv_obj_t* menu = ui_screen_get_menu();
    if (menu == NULL) {
        ESP_LOGE(TAG, "Menu not initialized. Call ui_main_init() first.");
        return;
    }
    lv_scr_load(menu);
}