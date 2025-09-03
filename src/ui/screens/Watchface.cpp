#include "Watchface.h"
#include <lvgl.h>
#include <cstdio>
#include <cstring>
#include <ctime>

#include "config/Theme.h"

static lv_obj_t *battery_arc;
static lv_obj_t *battery_percent_label;
static lv_obj_t *time_label;
static lv_obj_t *date_label;
static lv_obj_t *progress_bar;

static char current_time_str[16] = {0};

static int battery_level = 0;

static void update_battery_arc_and_text() {
    const int arc_value = battery_level * 360 / 100;
    lv_arc_set_value(battery_arc, arc_value);

    char batt_str[8];
    snprintf(batt_str, sizeof(batt_str), "%d%%", battery_level);
    lv_label_set_text(battery_percent_label, batt_str);
    lv_obj_set_style_text_color(battery_percent_label, COLOR_ACCENT, LV_PART_MAIN);
}

static void timer_cb(lv_timer_t *timer) {
    const time_t now = time(nullptr);
    const tm *local = localtime(&now);

    char time_buf[16];
    strftime(time_buf, sizeof(time_buf), "%H:%M", local);

    if (strcmp(time_buf, current_time_str) != 0) {
        strcpy(current_time_str, time_buf);
        lv_label_set_text(time_label, current_time_str);
    }

    char date_buf[32];
    strftime(date_buf, sizeof(date_buf), "%a, %b %d", local);
    lv_label_set_text(date_label, date_buf);

    const int progress = local->tm_sec * 100 / 59;
    lv_bar_set_value(progress_bar, progress, LV_ANIM_ON);

    update_battery_arc_and_text();
}

void Watchface::setup() {
    screenObj = lv_obj_create(nullptr);
    lv_obj_set_style_bg_color(screenObj, COLOR_BACKGROUND, LV_PART_MAIN);
    lv_obj_clear_flag(screenObj, LV_OBJ_FLAG_SCROLLABLE);

    battery_arc = lv_arc_create(screenObj);
    lv_obj_set_size(battery_arc, 230, 230);
    lv_arc_set_range(battery_arc, 0, 360);
    lv_arc_set_angles(battery_arc, 135, 45);
    lv_arc_set_value(battery_arc, 0);
    lv_obj_remove_style(battery_arc, nullptr, LV_PART_KNOB);
    lv_obj_set_style_arc_width(battery_arc, 5, LV_PART_MAIN);
    lv_obj_set_style_arc_width(battery_arc, 5, LV_PART_INDICATOR);
    lv_obj_set_style_arc_rounded(battery_arc, true, LV_PART_MAIN);
    lv_obj_clear_flag(battery_arc, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_align(battery_arc, LV_ALIGN_CENTER, 0, 0);

    battery_percent_label = lv_label_create(screenObj);
    lv_obj_set_style_text_font(battery_percent_label, &lv_font_montserrat_18, LV_PART_MAIN);
    lv_obj_align(battery_percent_label, LV_ALIGN_TOP_MID, 0, 20);
    lv_obj_set_style_text_color(battery_percent_label, COLOR_ACCENT, LV_PART_MAIN);

    time_label = lv_label_create(screenObj);
    lv_obj_set_style_text_color(time_label, COLOR_TEXT_MAIN, LV_PART_MAIN);
    lv_obj_set_style_text_font(time_label, &lv_font_montserrat_48, LV_PART_MAIN);
    lv_obj_align(time_label, LV_ALIGN_CENTER, 0, 0);

    date_label = lv_label_create(screenObj);
    lv_obj_set_style_text_color(date_label, COLOR_TEXT_SECONDARY, LV_PART_MAIN);
    lv_obj_set_style_text_font(date_label, &lv_font_montserrat_16, LV_PART_MAIN);
    lv_obj_align(date_label, LV_ALIGN_CENTER, 0, 80);

    progress_bar = lv_bar_create(screenObj);
    lv_obj_set_size(progress_bar, 100, 6);
    lv_obj_align(progress_bar, LV_ALIGN_CENTER, 0, 30);
    lv_bar_set_range(progress_bar, 0, 100);
    lv_obj_set_style_bg_color(progress_bar, COLOR_ACCENT_WEAK, LV_PART_MAIN);
    lv_obj_set_style_bg_color(progress_bar, COLOR_TEXT_MAIN, LV_PART_INDICATOR);

    const time_t now = time(nullptr);
    const tm *local = localtime(&now);
    strftime(current_time_str, sizeof(current_time_str), "%H:%M", local);
    lv_label_set_text(time_label, current_time_str);

    update_battery_arc_and_text();

    lv_timer_t *timer = lv_timer_create(timer_cb, 1000, nullptr);
    lv_timer_ready(timer);
}

void Watchface::update() {
}

void Watchface::onSwipeLeft() {
}

void Watchface::onSwipeRight() {
}
