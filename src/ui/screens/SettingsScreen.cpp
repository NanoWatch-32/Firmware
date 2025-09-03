#include "SettingsScreen.h"

#include <lvgl.h>

#include "config/Theme.h"

static lv_obj_t* main_label;

void SettingsScreen::setup() {
    screenObj = lv_obj_create(nullptr);
    lv_obj_set_style_bg_color(screenObj, COLOR_BACKGROUND, LV_PART_MAIN);
    lv_obj_clear_flag(screenObj, LV_OBJ_FLAG_SCROLLABLE);

    main_label = lv_label_create(screenObj);
    lv_label_set_text(main_label, "Settings");
    lv_obj_set_style_text_font(main_label, &lv_font_montserrat_20, LV_PART_MAIN);
    lv_obj_set_style_text_color(main_label, COLOR_TEXT_MAIN, LV_PART_MAIN);
    lv_obj_align(main_label, LV_ALIGN_TOP_MID, 0, 20);
}

void SettingsScreen::update() {
}

void SettingsScreen::onSwipeLeft() {
}

void SettingsScreen::onSwipeRight() {
}
