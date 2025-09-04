#include "MediaControlScreen.h"
#include <lvgl.h>
#include "config/Theme.h"
#include "bluetooth/BluetoothManager.h"

void MediaControlScreen::setup() {
    screenObj = lv_obj_create(nullptr);
    lv_obj_set_style_bg_color(screenObj, COLOR_BACKGROUND, LV_PART_MAIN);
    lv_obj_clear_flag(screenObj, LV_OBJ_FLAG_SCROLLABLE);

    titleLabel = lv_label_create(screenObj);
    lv_label_set_text(titleLabel, "Title: Unknown");
    lv_obj_set_style_text_font(titleLabel, &lv_font_montserrat_16, LV_PART_MAIN);
    lv_obj_align(titleLabel, LV_ALIGN_TOP_MID, 0, 20);

    artistLabel = lv_label_create(screenObj);
    lv_label_set_text(artistLabel, "Artist: Unknown");
    lv_obj_set_style_text_font(artistLabel, &lv_font_montserrat_16, LV_PART_MAIN);
    lv_obj_align(artistLabel, LV_ALIGN_TOP_MID, 0, 50);

    // Play Button
    playBtn = lv_btn_create(screenObj);
    lv_obj_set_size(playBtn, 60, 40);
    lv_obj_align(playBtn, LV_ALIGN_CENTER, -80, 40);
    lv_obj_t *playLabel = lv_label_create(playBtn);
    lv_label_set_text(playLabel, "Play");
    lv_obj_center(playLabel);
    lv_obj_add_event_cb(playBtn, [](lv_event_t *e) {
        static_cast<MediaControlScreen *>(lv_event_get_user_data(e))->sendAction(0x01);
    }, LV_EVENT_CLICKED, this);

    // Pause Button
    pauseBtn = lv_btn_create(screenObj);
    lv_obj_set_size(pauseBtn, 60, 40);
    lv_obj_align(pauseBtn, LV_ALIGN_CENTER, 0, 40);
    lv_obj_t *pauseLabel = lv_label_create(pauseBtn);
    lv_label_set_text(pauseLabel, "Pause");
    lv_obj_center(pauseLabel);
    lv_obj_add_event_cb(pauseBtn, [](lv_event_t *e) {
        static_cast<MediaControlScreen *>(lv_event_get_user_data(e))->sendAction(0x02);
    }, LV_EVENT_CLICKED, this);

    // Next Button
    nextBtn = lv_btn_create(screenObj);
    lv_obj_set_size(nextBtn, 60, 40);
    lv_obj_align(nextBtn, LV_ALIGN_CENTER, 80, 40);
    lv_obj_t *nextLabel = lv_label_create(nextBtn);
    lv_label_set_text(nextLabel, "Next");
    lv_obj_center(nextLabel);
    lv_obj_add_event_cb(nextBtn, [](lv_event_t *e) {
        static_cast<MediaControlScreen *>(lv_event_get_user_data(e))->sendAction(0x03);
    }, LV_EVENT_CLICKED, this);

    // Previous Button
    prevBtn = lv_btn_create(screenObj);
    lv_obj_set_size(prevBtn, 60, 40);
    lv_obj_align(prevBtn, LV_ALIGN_CENTER, 0, 90);
    lv_obj_t *prevLabel = lv_label_create(prevBtn);
    lv_label_set_text(prevLabel, "Prev");
    lv_obj_center(prevLabel);
    lv_obj_add_event_cb(prevBtn, [](lv_event_t *e) {
        static_cast<MediaControlScreen *>(lv_event_get_user_data(e))->sendAction(0x04);
    }, LV_EVENT_CLICKED, this);
}

void MediaControlScreen::sendAction(uint8_t action) {
    MediaCommandPacket packet(action);
    bluetooth_manager.send(packet);
}

void MediaControlScreen::update() {
}

void MediaControlScreen::onSwipeLeft() {
}

void MediaControlScreen::onSwipeRight() {
}
