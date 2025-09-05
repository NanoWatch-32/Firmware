#include "MediaControlScreen.h"
#include <lvgl.h>
#include "config/Theme.h"
#include "bluetooth/BluetoothManager.h"
#include "protocol/packet/MediaCommandPacket.h"
#include "protocol/packet/MediaInfoPacket.h"

void MediaControlScreen::init() {
    bluetooth_manager.listen(PacketType::MEDIA_INFO, [this](const Packet &packet) {
        const MediaInfoPacket *mediaPacket = dynamic_cast<const MediaInfoPacket *>(&packet);

        if (mediaPacket) {
            updateMetadata(
                mediaPacket->title,
                mediaPacket->artist,
                mediaPacket->album,
                mediaPacket->duration,
                mediaPacket->position,
                mediaPacket->isPlaying
            );
        }
    });
}

void MediaControlScreen::setup() {
    screenObj = lv_obj_create(nullptr);
    lv_obj_set_style_bg_color(screenObj, COLOR_BACKGROUND, LV_PART_MAIN);
    lv_obj_clear_flag(screenObj, LV_OBJ_FLAG_SCROLLABLE);

    titleLabel = lv_label_create(screenObj);
    lv_label_set_text(titleLabel, "Title: Unknown");
    lv_obj_set_style_text_font(titleLabel, &lv_font_montserrat_16, LV_PART_MAIN);
    lv_obj_align(titleLabel, LV_ALIGN_TOP_MID, 0, 10);

    artistLabel = lv_label_create(screenObj);
    lv_label_set_text(artistLabel, "Artist: Unknown");
    lv_obj_set_style_text_font(artistLabel, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_align(artistLabel, LV_ALIGN_TOP_MID, 0, 35);

    albumLabel = lv_label_create(screenObj);
    lv_label_set_text(albumLabel, "Album: Unknown");
    lv_obj_set_style_text_font(albumLabel, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_align(albumLabel, LV_ALIGN_TOP_MID, 0, 55);

    progressBar = lv_bar_create(screenObj);
    lv_obj_set_size(progressBar, 200, 15);
    lv_obj_align(progressBar, LV_ALIGN_TOP_MID, 0, 80);
    lv_bar_set_range(progressBar, 0, 1000);
    lv_bar_set_value(progressBar, 0, LV_ANIM_OFF);

    positionLabel = lv_label_create(screenObj);
    lv_label_set_text(positionLabel, "0:00");
    lv_obj_set_style_text_font(positionLabel, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_align(positionLabel, LV_ALIGN_TOP_MID, -90, 100);

    durationLabel = lv_label_create(screenObj);
    lv_label_set_text(durationLabel, "0:00");
    lv_obj_set_style_text_font(durationLabel, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_align(durationLabel, LV_ALIGN_TOP_MID, 90, 100);

    playBtn = lv_btn_create(screenObj);
    lv_obj_set_size(playBtn, 60, 40);
    lv_obj_align(playBtn, LV_ALIGN_CENTER, -80, 40);
    lv_obj_t *playLabel = lv_label_create(playBtn);
    lv_label_set_text(playLabel, "Play");
    lv_obj_center(playLabel);
    lv_obj_add_event_cb(playBtn, [](lv_event_t *e) {
        static_cast<MediaControlScreen *>(lv_event_get_user_data(e))->sendAction(0x01);
    }, LV_EVENT_CLICKED, this);

    pauseBtn = lv_btn_create(screenObj);
    lv_obj_set_size(pauseBtn, 60, 40);
    lv_obj_align(pauseBtn, LV_ALIGN_CENTER, 0, 40);
    lv_obj_t *pauseLabel = lv_label_create(pauseBtn);
    lv_label_set_text(pauseLabel, "Pause");
    lv_obj_center(pauseLabel);
    lv_obj_add_event_cb(pauseBtn, [](lv_event_t *e) {
        static_cast<MediaControlScreen *>(lv_event_get_user_data(e))->sendAction(0x02);
    }, LV_EVENT_CLICKED, this);

    nextBtn = lv_btn_create(screenObj);
    lv_obj_set_size(nextBtn, 60, 40);
    lv_obj_align(nextBtn, LV_ALIGN_CENTER, 80, 40);
    lv_obj_t *nextLabel = lv_label_create(nextBtn);
    lv_label_set_text(nextLabel, "Next");
    lv_obj_center(nextLabel);
    lv_obj_add_event_cb(nextBtn, [](lv_event_t *e) {
        static_cast<MediaControlScreen *>(lv_event_get_user_data(e))->sendAction(0x03);
    }, LV_EVENT_CLICKED, this);

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

void MediaControlScreen::onProgressTimer() {
    uint32_t now = millis();
    uint32_t delta = now - lastUpdateMillis;
    lastUpdateMillis = now;

    currentPosition += delta;

    if (currentPosition > totalDuration) {
        currentPosition = totalDuration;
    }

    updateProgressDisplay();
}


void MediaControlScreen::updateMetadata(const std::string &title, const std::string &artist,
                                        const std::string &album, uint32_t duration,
                                        uint32_t position, bool isPlaying) {
    lv_label_set_text_fmt(titleLabel, "Title: %s", title.c_str());
    lv_label_set_text_fmt(artistLabel, "Artist: %s", artist.c_str());
    lv_label_set_text_fmt(albumLabel, "Album: %s", album.c_str());

    totalDuration = duration;
    currentPosition = position;
    playing = isPlaying;

    updateProgressDisplay();

    if (playing) {
        if (!progressTimer) {
            lastUpdateMillis = millis();
            progressTimer = lv_timer_create([](lv_timer_t *t) {
                static_cast<MediaControlScreen *>(t->user_data)->onProgressTimer();
            }, 1000 / 30, this);
        }
    } else {
        if (progressTimer) {
            lv_timer_del(progressTimer);
            progressTimer = nullptr;
        }
    }
}


void MediaControlScreen::updateProgressDisplay() const {
    if (totalDuration > 0) {
        uint16_t progress = (currentPosition * 1000) / totalDuration;
        lv_bar_set_value(progressBar, progress, LV_ANIM_ON);
    } else {
        lv_bar_set_value(progressBar, 0, LV_ANIM_OFF);
    }

    lv_label_set_text(positionLabel, formatTime(currentPosition).c_str());
    lv_label_set_text(durationLabel, formatTime(totalDuration).c_str());
}

std::string MediaControlScreen::formatTime(uint32_t milliseconds) {
    uint32_t seconds = milliseconds / 1000;
    uint32_t minutes = seconds / 60;
    seconds = seconds % 60;

    char buffer[10];
    snprintf(buffer, sizeof(buffer), "%d:%02d", minutes, seconds);
    return std::string(buffer);
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
