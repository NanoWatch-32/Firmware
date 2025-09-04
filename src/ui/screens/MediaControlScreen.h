#pragma once
#include <WString.h>

#include "ui/Screen.h"

class MediaControlScreen : public Screen {
public:
    void setup() override;
    void update() override;
    void onSwipeLeft() override;
    void onSwipeRight() override;

    void updateMetadata(const String& title, const String& artist,
                   const String& album, uint32_t duration,
                   uint32_t position, bool isPlaying);

private:
    void sendAction(uint8_t action);

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

    void updateProgressDisplay();
    String formatTime(uint32_t milliseconds);
};
