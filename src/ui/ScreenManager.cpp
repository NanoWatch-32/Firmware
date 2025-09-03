#include "ScreenManager.h"
#include <lvgl.h>

ScreenManager screenManager;

static void gesture_event_cb(lv_event_t *e) {
    const lv_dir_t dir = lv_indev_get_gesture_dir(lv_event_get_indev(e));
    Screen *screen = static_cast<Screen *>(lv_event_get_user_data(e));
    if (!screen) return;

    switch (dir) {
        case LV_DIR_TOP:
            screenManager.nextScreen();
            break;
        case LV_DIR_BOTTOM:
            screenManager.previousScreen();
            break;
        case LV_DIR_LEFT:
            screen->onSwipeLeft();
            break;
        case LV_DIR_RIGHT:
            screen->onSwipeRight();
            break;
        default:
            break;
    }
}

static lv_scr_load_anim_t mapDirectionToAnimType(const lv_dir_t dir) {
    switch (dir) {
        case LV_DIR_LEFT:
            return LV_SCR_LOAD_ANIM_MOVE_LEFT;
        case LV_DIR_RIGHT:
            return LV_SCR_LOAD_ANIM_MOVE_RIGHT;
        case LV_DIR_TOP:
            return LV_SCR_LOAD_ANIM_MOVE_TOP;
        case LV_DIR_BOTTOM:
            return LV_SCR_LOAD_ANIM_MOVE_BOTTOM;
        // ReSharper disable once CppDFAUnreachableCode
        default: return LV_SCR_LOAD_ANIM_NONE;
    }
}

static void animateScreenTransition(lv_obj_t *newScreen, const lv_dir_t direction) {
    if (!newScreen) return;

    const lv_scr_load_anim_t animType = mapDirectionToAnimType(direction);
    lv_scr_load_anim(newScreen, animType, 150, 0, false);
}

void ScreenManager::addScreen(Screen *screen) {
    screens.push_back(screen);
}

void ScreenManager::setCurrentScreen(const size_t index) {
    if (index >= screens.size()) return;

    currentIndex = index;
    Screen *screen = screens[currentIndex];

    screen->setup();
    lv_obj_add_event_cb(screen->getScreenObj(), gesture_event_cb, LV_EVENT_GESTURE, screen);

    lv_scr_load(screen->getScreenObj());
}

void ScreenManager::transitionToScreen(const size_t index, const lv_dir_t direction) {
    if (index >= screens.size() || index == currentIndex) return;

    Screen *nextScreen = screens[index];
    nextScreen->setup();

    lv_obj_add_event_cb(nextScreen->getScreenObj(), gesture_event_cb, LV_EVENT_GESTURE, nextScreen);

    animateScreenTransition(nextScreen->getScreenObj(), direction);

    currentIndex = index;
}

void ScreenManager::nextScreen() {
    if (screens.empty()) return;
    const size_t nextIndex = (currentIndex + 1) % screens.size();
    transitionToScreen(nextIndex, LV_DIR_TOP);
}

void ScreenManager::previousScreen() {
    if (screens.empty()) return;
    const size_t prevIndex = (currentIndex + screens.size() - 1) % screens.size();
    transitionToScreen(prevIndex, LV_DIR_BOTTOM);
}

void ScreenManager::swipeLeftScreen() {
    if (screens.empty()) return;
    const size_t nextIndex = (currentIndex + 1) % screens.size();
    transitionToScreen(nextIndex, LV_DIR_LEFT);
}

void ScreenManager::swipeRightScreen() {
    if (screens.empty()) return;
    const size_t prevIndex = (currentIndex + screens.size() - 1) % screens.size();
    transitionToScreen(prevIndex, LV_DIR_RIGHT);
}

void ScreenManager::update() const {
    if (!screens.empty()) {
        screens[currentIndex]->update();
    }
}
