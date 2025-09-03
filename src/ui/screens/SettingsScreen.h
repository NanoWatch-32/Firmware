#pragma once
#include "ui/Screen.h"

class SettingsScreen : public Screen {
public:
    void setup() override;

    void update() override;

    void onSwipeLeft() override;

    void onSwipeRight() override;
};
