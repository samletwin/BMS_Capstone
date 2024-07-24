#include "ui_animations.h"

UIAnimations::UIAnimations() {}

UIAnimations::~UIAnimations() {
    // Clean up any ongoing animations if necessary
}

void UIAnimations::addFadeAnimation(lv_obj_t* obj, uint32_t duration, lv_opa_t startOpacity, lv_opa_t endOpacity) {
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_values(&a, startOpacity, endOpacity);
    lv_anim_set_time(&a, duration);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)[](void* obj, int32_t v) {
        lv_obj_set_style_opa((lv_obj_t*)obj, v, 0);
    });
    lv_anim_start(&a);
    animations.push_back(a);
}
void UIAnimations::addMoveAnimation(lv_obj_t* obj, uint32_t duration, lv_coord_t startX, lv_coord_t startY, lv_coord_t endX, lv_coord_t endY) {
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_values(&a, startX, endX);
    lv_anim_set_time(&a, duration);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_x);
    lv_anim_start(&a);
    animations.push_back(a);

    lv_anim_t b;
    lv_anim_init(&b);
    lv_anim_set_var(&b, obj);
    lv_anim_set_values(&b, startY, endY);
    lv_anim_set_time(&b, duration);
    lv_anim_set_exec_cb(&b, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_start(&b);
    animations.push_back(b);
}

void UIAnimations::addScaleAnimation(lv_obj_t* obj, uint32_t duration, lv_coord_t startScale, lv_coord_t endScale) {
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_values(&a, startScale, endScale);
    lv_anim_set_time(&a, duration);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)[](void* obj, int32_t v) {
        lv_obj_set_style_transform_scale((lv_obj_t*)obj, v, 0);
    });
    lv_anim_start(&a);
    animations.push_back(a);
}

void UIAnimations::addCustomAnimation(lv_obj_t* obj, uint32_t duration, lv_anim_exec_xcb_t exec_cb, int32_t start, int32_t end) {
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_values(&a, start, end);
    lv_anim_set_time(&a, duration);
    lv_anim_set_exec_cb(&a, exec_cb);
    lv_anim_start(&a);
    animations.push_back(a);
}