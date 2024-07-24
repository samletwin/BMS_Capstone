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
    
    bool init();  // New initialization method
    void show();
    void stopAnimations();
    UIManager& getUIManager();

private:
    UIScreen mainScreen;
    UIManager uiManager;
    UIAnimations animations;
    
    lv_obj_t* warningLabel;
    lv_obj_t* exclamationIcon;
    bool animationPlaying;
    bool initialized;

    void createUI(lv_obj_t* parent);
    void updateUI();
    void startAnimations();
    static void staticUpdateFunction(void* userData);
};

#endif // UI_MAIN_H