#include "ui_screen.h"

UIScreen::UIScreen() : parentScreen(nullptr), createFunction(nullptr), updateFunction(nullptr), updateUserData(nullptr) {}

UIScreen::~UIScreen() {
    // Clean up any allocated resources
}

void UIScreen::setCreateFunction(CreateFunction createFunc) {
    createFunction = createFunc;
}

void UIScreen::setUpdateFunction(UpdateFunction updateFunc, void* userData) {
    updateFunction = updateFunc;
    updateUserData = userData;
}

void UIScreen::createScreen() {
    if (parentScreen == nullptr) {
        parentScreen = lv_obj_create(nullptr);
    }
    if (createFunction) {
        createFunction(parentScreen);
    }
}

void UIScreen::setScreen(lv_obj_t* screen) {
    parentScreen = screen;
}

void UIScreen::updateScreen() {
    if (updateFunction) {
        updateFunction(updateUserData);
    }
}

lv_obj_t* UIScreen::getScreen() const {
    return parentScreen;
}