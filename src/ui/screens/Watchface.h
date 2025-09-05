#pragma once
#include "ui/Screen.h"

class Watchface : public Screen {
public:
    void init() override;

    void setup() override;

    void update() override;

    void onSwipeLeft() override;

    void onSwipeRight() override;
};
