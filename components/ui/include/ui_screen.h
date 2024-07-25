#ifndef UI_SCREEN_H
#define UI_SCREEN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"

void ui_screen_init(void);
lv_obj_t* ui_screen_get_menu(void);
void ui_screen_create_menu(void);

#ifdef __cplusplus
} /* extern C */
#endif

#endif // UI_SCREEN_H