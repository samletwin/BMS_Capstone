#include "ui_main.h"
#include "esp_log.h"

static const char* TAG = "UIMain";

UIMain::UIMain() 
    : warningLabel(nullptr), 
      exclamationIcon(nullptr), 
      animationPlaying(true), 
      initialized(false) {
    ESP_LOGI(TAG, "UIMain constructor called");
}

UIMain::~UIMain() {
    // Cleanup if necessary
}

bool UIMain::init() {
    ESP_LOGI(TAG, "Initializing UIMain");
    
    lv_obj_t* screen = lv_obj_create(nullptr);
    if (screen == nullptr) {
        ESP_LOGE(TAG, "Failed to create screen object");
        return false;
    }
    
    lv_obj_set_user_data(screen, this);
    
    mainScreen.setCreateFunction([](lv_obj_t* parent) { 
        UIMain* self = static_cast<UIMain*>(lv_obj_get_user_data(parent));
        if (self) {
            self->createUI(parent);
        }
    });
    mainScreen.setUpdateFunction(staticUpdateFunction, this);
    
    mainScreen.setScreen(screen);
    
    initialized = true;
    ESP_LOGI(TAG, "UIMain initialized successfully");
    return true;
}

void UIMain::show() {
    if (!initialized) {
        ESP_LOGE(TAG, "UIMain not initialized. Call init() first.");
        return;
    }
    
    if (mainScreen.getScreen() == nullptr) {
        ESP_LOGE(TAG, "Screen is null");
        return;
    }
    createUI(mainScreen.getScreen());
    lv_scr_load(mainScreen.getScreen());
}

void UIMain::createUI(lv_obj_t* parent) {
    if (parent == nullptr) {
        ESP_LOGE(TAG, "Parent screen is null");
        return;
    }

    // Create the warning label
    warningLabel = lv_label_create(parent);
    if (warningLabel == nullptr) {
        ESP_LOGE(TAG, "Failed to create warning label");
        return;
    }
    lv_label_set_text(warningLabel, "Lost Connection to BMS\nPlease reconnect");
    lv_obj_align(warningLabel, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_text_font(warningLabel, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(warningLabel, lv_color_hex(0xFF0000), 0);

    // Create the exclamation icon
    exclamationIcon = lv_label_create(parent);
    if (exclamationIcon == nullptr) {
        ESP_LOGE(TAG, "Failed to create exclamation icon");
        return;
    }
    lv_label_set_text(exclamationIcon, LV_SYMBOL_WARNING);
    lv_obj_set_style_text_font(exclamationIcon, &lv_font_montserrat_48, 0);
    lv_obj_set_style_text_color(exclamationIcon, lv_color_hex(0xFF0000), 0);
    lv_obj_align(exclamationIcon, LV_ALIGN_TOP_MID, 0, 20);

    // Start animations
    startAnimations();
}

void UIMain::updateUI() {
    if (!animationPlaying) {
        lv_anim_del(warningLabel, nullptr);
        lv_anim_del(exclamationIcon, nullptr);
    }
}

void UIMain::startAnimations() {
    // Pulsing animation for the warning label
    animations.addCustomAnimation(warningLabel, 1000, 
        [](void* var, int32_t v) { lv_obj_set_style_text_opa(static_cast<lv_obj_t*>(var), v, 0); },
        100, 255);

    // Rotating animation for the exclamation icon
    animations.addCustomAnimation(exclamationIcon, 2000,
        [](void* var, int32_t v) { lv_obj_set_style_transform_angle(static_cast<lv_obj_t*>(var), v, 0); },
        0, 3600);
}

void UIMain::stopAnimations() {
    animationPlaying = false;
}

UIManager& UIMain::getUIManager() {
    return uiManager;
}


void UIMain::staticUpdateFunction(void* userData) {
    UIMain* self = static_cast<UIMain*>(userData);
    if (self == nullptr) {
        ESP_LOGE(TAG, "Invalid user data in staticUpdateFunction");
        return;
    }
    self->updateUI();
}