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

    void addFadeAnimation(lv_obj_t* obj, uint32_t duration, lv_opa_t startOpacity, lv_opa_t endOpacity);
    void addMoveAnimation(lv_obj_t* obj, uint32_t duration, lv_coord_t startX, lv_coord_t startY, lv_coord_t endX, lv_coord_t endY);
    void addScaleAnimation(lv_obj_t* obj, uint32_t duration, lv_coord_t startScale, lv_coord_t endScale);
    void addCustomAnimation(lv_obj_t* obj, uint32_t duration, lv_anim_exec_xcb_t exec_cb, int32_t start, int32_t end);

private:
    std::vector<lv_anim_t> animations;
};

#endif // UI_ANIMATIONS_H