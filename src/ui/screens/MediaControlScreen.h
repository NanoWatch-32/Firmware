#pragma once

#include "lvgl_private.h"
#include <string>
#include "ui/Screen.h"

class MediaControlScreen : public Screen {
public:
    void init() override;
    void setup() override;

    void onProgressTimer();

    void update() override;
    void onSwipeLeft() override;
    void onSwipeRight() override;

    void updateMetadata(const std::string& title, const std::string& artist,
                   const std::string& album, uint32_t duration,
                   uint32_t position, bool isPlaying);

private:
    static void sendAction(uint8_t action);

    lv_obj_t* titleLabel;
    lv_obj_t* artistLabel;
    lv_obj_t* albumLabel;
    lv_obj_t* progressBar;
    lv_obj_t* durationLabel;
    lv_obj_t* positionLabel;
    lv_obj_t* playBtn;
    lv_obj_t* pauseBtn;
    lv_obj_t* nextBtn;
    lv_obj_t* prevBtn;

    uint32_t totalDuration = 0;
    uint32_t currentPosition = 0;
    bool playing = false;

    lv_timer_t* progressTimer = nullptr;
    uint32_t lastUpdateMillis = 0;

    void updateProgressDisplay() const;

    static std::string formatTime(uint32_t milliseconds);
};
