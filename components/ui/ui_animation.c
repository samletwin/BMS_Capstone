#include "ui_animation.h"
#include "esp_log.h"

static const char* TAG = "UI_ANIMATION";

void ui_animation_init(void)
{
    ESP_LOGI(TAG, "Initializing UI Animation");
}

void ui_animation_add_pulse(lv_obj_t* obj, uint32_t duration)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_values(&a, LV_OPA_TRANSP, LV_OPA_COVER);
    lv_anim_set_time(&a, duration);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_style_opa);
    lv_anim_start(&a);
}

void ui_animation_add_rotate(lv_obj_t* obj, uint32_t duration)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_values(&a, 0, 3600);
    lv_anim_set_time(&a, duration);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_img_set_angle);
    lv_anim_start(&a);
}

void ui_animation_stop_all(void)
{
    lv_anim_del_all();
}