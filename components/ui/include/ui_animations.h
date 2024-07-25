#ifndef UI_ANIMATIONS_H
#define UI_ANIMATIONS_H

#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <lvgl.h>
#include <vector>

class UIAnimations {
public:
    UIAnimations();
    ~UIAnimations();

    void addPulseAnimation(lv_obj_t* obj, uint32_t duration);
    void addRotateAnimation(lv_obj_t* obj, uint32_t duration);
    void stopAllAnimations();

private:
    std::vector<lv_anim_t*> animations;
};

#endif // UI_ANIMATIONS_H