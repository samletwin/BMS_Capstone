#ifndef UI_MAIN_H
#define UI_MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"

void ui_main_init(void);
void ui_main_show(void);
void ui_main_tick(void);

#ifdef __cplusplus
} /* extern C */
#endif

#endif // UI_MAIN_H