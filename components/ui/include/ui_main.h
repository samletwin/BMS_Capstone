#ifndef UI_MAIN_H
#define UI_MAIN_H

#include "ui_screen.h"
#include "ui_manager.h"
#include "ui_animations.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <lvgl.h>
#include <stdbool.h>

class UIMain {
public:
    UIMain();
    ~UIMain();
    
    bool init();
    void show();
    UIManager& getUIManager();

private:
    UIManager uiManager;
    lv_obj_t* menu;
    lv_obj_t* packDataPage;
    lv_obj_t* cellDataPage;
    lv_obj_t* settingsPage;

    void createMenu();
    void createPackDataPage();
    void createCellDataPage();
    void createSettingsPage();
    static void menuBackEventHandler(lv_event_t* e);
};

#endif // UI_MAIN_H