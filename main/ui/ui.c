
// #include "ui.h"
// #include "screens.h"
// #include "styles.h"
// #include "esp_log.h"

// static int16_t currentScreen = -1;



// void loadScreen(enum ScreensEnum screenId) {
//     if (screenId == SCREEN_ID_MAIN_SINGLE_CELL)
//         lv_scr_load(objects.main_single_cell);
//     else if (screenId == SCREEN_ID_SETTINGS)
//         lv_scr_load(objects.settings);
//     else if (screenId == SCREEN_ID_KEYPAD_EDIT)
//         lv_scr_load(objects.keypad_screen);
//     else if (screenId == SCREEN_ID_WALLPAPER)
//         lv_scr_load(objects.wallpaper_screen);
//     else
//         ESP_LOGE("UI", "Unable to load screen for screen ID %u, because the object for loading is not defined.", screenId);
// }

// void ui_init() {
//     create_screens();
//     loadScreen(SCREEN_ID_WALLPAPER);
// }

// void ui_tick() {
//     // tick_screen(currentScreen);
// }

