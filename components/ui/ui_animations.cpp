#include "ui_animations.h"
#include "esp_log.h"

static const char* TAG = "UIAnimations";

UIAnimations::UIAnimations() {}

UIAnimations::~UIAnimations() {
    stopAllAnimations();
}

void UIAnimations::addPulseAnimation(lv_obj_t* obj, uint32_t duration) {
    lv_anim_t* a = new lv_anim_t;
    lv_anim_init(a);
    lv_anim_set_var(a, obj);
    lv_anim_set_values(a, LV_OPA_TRANSP, LV_OPA_COVER);
    lv_anim_set_time(a, duration);
    lv_anim_set_repeat_count(a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_exec_cb(a, [](void* obj, int32_t value) {
        lv_obj_set_style_opa(static_cast<lv_obj_t*>(obj), value, 0);
    });
    lv_anim_start(a);
    animations.push_back(a);
}

void UIAnimations::addRotateAnimation(lv_obj_t* obj, uint32_t duration) {
    lv_anim_t* a = new lv_anim_t;
    lv_anim_init(a);
    lv_anim_set_var(a, obj);
    lv_anim_set_values(a, 0, 3600);
    lv_anim_set_time(a, duration);
    lv_anim_set_repeat_count(a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_exec_cb(a, [](void* obj, int32_t value) {
        lv_obj_set_style_transform_angle(static_cast<lv_obj_t*>(obj), value, 0);
    });
    lv_anim_start(a);
    animations.push_back(a);
}

void UIAnimations::stopAllAnimations() {
    for (auto anim : animations) {
        lv_anim_del(anim->var, nullptr);
        delete anim;
    }
    animations.clear();
}