#include <Arduino.h>
#include <CST816S.h>
#include <config/CST816S_pin_config.h>
#include <lvgl.h>
#include <TFT_eSPI.h>
#include <sys/time.h>
#include <esp_heap_caps.h>

#include "bluetooth/BluetoothManager.h"
#include "ui/ScreenManager.h"
#include "ui/screens/BluetoothScreen.h"
#include "ui/screens/MediaControlScreen.h"
#include "ui/screens/Watchface.h"
#include "ui/screens/SettingsScreen.h"

// Display settings
constexpr uint16_t SCREEN_WIDTH = 240;
constexpr uint16_t SCREEN_HEIGHT = 240;
constexpr size_t DRAW_BUF_SIZE = SCREEN_WIDTH * SCREEN_HEIGHT / 8 * (LV_COLOR_DEPTH / 8);
static lv_color_t draw_buf1[DRAW_BUF_SIZE];
static lv_color_t draw_buf2[DRAW_BUF_SIZE];

TFT_eSPI tft = TFT_eSPI(SCREEN_WIDTH, SCREEN_HEIGHT);

// Touch
CST816S touch(TOUCH_SDA, TOUCH_SCL, TOUCH_RST, TOUCH_IRQ);

unsigned long lastInteractionTime = 0;
bool sleeping = false;

#define SLEEP_TIMEOUT 15000
#define TFT_BACKLIGHT_PIN 2

constexpr size_t MEMORY_THRESHOLD = 15000; // 15 KB

constexpr unsigned long MEMORY_CHECK_INTERVAL = 5000;
unsigned long lastMemoryCheckTime = 0;

// Resolve potential power conflicts
void disableBatteryMonitoringDuringBoot() {
    pinMode(1, INPUT);
    delay(100);
}

void lvDisplayFlush(lv_display_t *disp, const lv_area_t *area, uint8_t *colorMap) {
    const int32_t w = area->x2 - area->x1 + 1;
    const int32_t h = area->y2 - area->y1 + 1;

    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.pushColors(reinterpret_cast<uint16_t *>(colorMap), w * h, true);
    tft.endWrite();

    lv_display_flush_ready(disp);
}

void fadeOutBacklight() {
    for (int duty = 255; duty >= 0; duty -= 5) {
        analogWrite(TFT_BACKLIGHT_PIN, duty);
        delay(10);
    }
    analogWrite(TFT_BACKLIGHT_PIN, 0);
}

void fadeInBacklight() {
    for (int duty = 0; duty <= 255; duty += 5) {
        analogWrite(TFT_BACKLIGHT_PIN, duty);
        delay(10);
    }
    analogWrite(TFT_BACKLIGHT_PIN, 255);
}

void goToSleep() {
    Serial.println("Going to sleep...");
    fadeOutBacklight();
    sleeping = true;
}

void wakeUp() {
    Serial.println("Waking up...");
    fadeInBacklight();
    sleeping = false;
    lastInteractionTime = millis();
}

void lvTouchRead(lv_indev_t *indev, lv_indev_data_t *data) {
    if (!touch.available()) {
        data->state = LV_INDEV_STATE_RELEASED;
    } else {
        data->state = LV_INDEV_STATE_PRESSED;
        data->point.x = touch.data.x;
        data->point.y = touch.data.y;

        if (sleeping) {
            wakeUp();
        }

        lastInteractionTime = millis();
    }
}

uint32_t lvTick() { return millis(); }

void setInitialTime() {
    tm tm_time{};
    tm_time.tm_year = 2025 - 1900;
    tm_time.tm_mon = 9 - 1;
    tm_time.tm_mday = 2;
    tm_time.tm_hour = 14;
    tm_time.tm_min = 30;
    tm_time.tm_sec = 0;

    const time_t t = mktime(&tm_time);
    const timeval now = {.tv_sec = t};
    settimeofday(&now, nullptr);

    Serial.println("System time set.");
}

void checkMemoryAndRebootIfNeeded() {
    size_t freeHeap = ESP.getFreeHeap();
    size_t freePsram = heap_caps_get_free_size(MALLOC_CAP_SPIRAM);

    if (freeHeap < MEMORY_THRESHOLD || freePsram < MEMORY_THRESHOLD) {
        Serial.println("WARNING: Low memory detected (heap or PSRAM), rebooting...");
        delay(100);
        ESP.restart();
    }
}

void setup() {
    disableBatteryMonitoringDuringBoot();

    Serial.begin(115200);
    touch.begin();

    setInitialTime();

    lv_init();
    lv_tick_set_cb(lvTick);

    // Initialize TFT
    tft.begin();
    tft.setRotation(0);

    lv_display_t *disp = lv_display_create(SCREEN_WIDTH, SCREEN_HEIGHT);
    lv_display_set_flush_cb(disp, lvDisplayFlush);
    lv_display_set_buffers(disp, draw_buf1, draw_buf2, DRAW_BUF_SIZE, LV_DISPLAY_RENDER_MODE_PARTIAL);

    lv_indev_t *indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, lvTouchRead);

    screenManager.addScreen(new Watchface());
    screenManager.addScreen(new SettingsScreen());
    screenManager.addScreen(new BluetoothScreen());
    screenManager.addScreen(new MediaControlScreen());
    screenManager.setCurrentScreen(0);

    pinMode(TFT_BACKLIGHT_PIN, OUTPUT);
    digitalWrite(TFT_BACKLIGHT_PIN, HIGH);
    lastInteractionTime = millis();
    lastMemoryCheckTime = millis();

    bluetooth_manager.begin("NanoWatch");

    Serial.println("Setup complete!");
}

void loop() {
    screenManager.update();
    lv_timer_handler();
    delay(5);

    if (!sleeping && millis() - lastInteractionTime > SLEEP_TIMEOUT) {
        goToSleep();
    }

    unsigned long now = millis();
    if (now - lastMemoryCheckTime >= MEMORY_CHECK_INTERVAL) {
        lastMemoryCheckTime = now;
        checkMemoryAndRebootIfNeeded();
    }
}
