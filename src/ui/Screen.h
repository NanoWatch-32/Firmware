#pragma once
#include <lvgl.h>

class Screen {
public:
    virtual ~Screen() = default;

    virtual void setup() = 0;

    virtual void update() = 0;

    virtual void onSwipeLeft() {
    }

    virtual void onSwipeRight() {
    }

    lv_obj_t *getScreenObj() const { return screenObj; }

protected:
    lv_obj_t *screenObj = nullptr;
};
