#pragma once
#include <vector>
#include "Screen.h"

class ScreenManager {
public:
    void addScreen(Screen *screen);

    void setCurrentScreen(size_t index);

    void nextScreen();

    void previousScreen();

    void swipeLeftScreen();

    void swipeRightScreen();

    void update() const;

private:
    std::vector<Screen *> screens;
    size_t currentIndex = 0;

    void transitionToScreen(size_t index, lv_dir_t direction);
};

extern ScreenManager screenManager;
