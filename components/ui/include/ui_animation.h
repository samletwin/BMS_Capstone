#ifndef UI_ANIMATION_H
#define UI_ANIMATION_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"

void ui_animation_init(void);
void ui_animation_add_pulse(lv_obj_t* obj, uint32_t duration);
void ui_animation_add_rotate(lv_obj_t* obj, uint32_t duration);
void ui_animation_stop_all(void);

#ifdef __cplusplus
} /* extern C */
#endif

#endif // UI_ANIMATION_H