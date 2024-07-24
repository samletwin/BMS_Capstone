#ifndef UI_SCREEN_H
#define UI_SCREEN_H

#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <lvgl.h>
#include <vector>

class UIScreen {
public:
    UIScreen();
    ~UIScreen();

    typedef void (*CreateFunction)(lv_obj_t*);
    typedef void (*UpdateFunction)(void*);

    void setCreateFunction(CreateFunction createFunc);
    void setUpdateFunction(UpdateFunction updateFunc, void* userData);
    void createScreen();
    void setScreen(lv_obj_t* screen);
    void updateScreen();
    lv_obj_t* getScreen() const;

private:
    lv_obj_t* parentScreen;
    std::vector<lv_obj_t*> screenObjects;
    CreateFunction createFunction;
    UpdateFunction updateFunction;
    void* updateUserData;
};

#endif // UI_SCREEN_H