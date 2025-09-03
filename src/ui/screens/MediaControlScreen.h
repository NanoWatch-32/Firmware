#pragma once
#include "ui/Screen.h"

class MediaControlScreen : public Screen {
public:
    void setup() override;
    void update() override;
    void onSwipeLeft() override;
    void onSwipeRight() override;

private:
    void sendAction(uint8_t action);

    lv_obj_t* titleLabel;
    lv_obj_t* artistLabel;
    lv_obj_t* playBtn;
    lv_obj_t* pauseBtn;
    lv_obj_t* nextBtn;
    lv_obj_t* prevBtn;
};
