#include "BluetoothScreen.h"
#include <lvgl.h>
#include "config/Theme.h"

void BluetoothScreen::setup() {
    screenObj = lv_obj_create(nullptr);
    lv_obj_set_style_bg_color(screenObj, COLOR_BACKGROUND, LV_PART_MAIN);
    lv_obj_clear_flag(screenObj, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t* title = lv_label_create(screenObj);
    lv_label_set_text(title, "Bluetooth");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_20, LV_PART_MAIN);
    lv_obj_set_style_text_color(title, COLOR_TEXT_MAIN, LV_PART_MAIN);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);

    m_statusLabel = lv_label_create(screenObj);
    lv_obj_set_style_text_font(m_statusLabel, &lv_font_montserrat_16, LV_PART_MAIN);
    lv_obj_set_style_text_color(m_statusLabel, COLOR_TEXT_MAIN, LV_PART_MAIN);
    lv_obj_align(m_statusLabel, LV_ALIGN_TOP_MID, 0, 40);

    // Connection status label
    m_connectionLabel = lv_label_create(screenObj);
    lv_obj_set_style_text_font(m_connectionLabel, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_set_style_text_color(m_connectionLabel, COLOR_TEXT_MAIN, LV_PART_MAIN);
    lv_obj_align(m_connectionLabel, LV_ALIGN_TOP_MID, 0, 70);

    // Advertising switch
    lv_obj_t* switchLabel = lv_label_create(screenObj);
    lv_label_set_text(switchLabel, "Advertising:");
    lv_obj_set_style_text_font(switchLabel, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_set_style_text_color(switchLabel, COLOR_TEXT_MAIN, LV_PART_MAIN);
    lv_obj_align(switchLabel, LV_ALIGN_TOP_LEFT, 20, 100);

    m_advertisingSwitch = lv_switch_create(screenObj);
    lv_obj_align(m_advertisingSwitch, LV_ALIGN_TOP_RIGHT, -20, 100);
    lv_obj_set_user_data(m_advertisingSwitch, this);
    lv_obj_add_event_cb(m_advertisingSwitch, onAdvertisingSwitchEvent, LV_EVENT_VALUE_CHANGED, nullptr);

    // Initialize Bluetooth manager
    bluetooth_manager.onConnect([this](NimBLEServer* pServer) {
        updateStatus();
    });

    // Initial status update
    updateStatus();
}

void BluetoothScreen::update() {
    static uint32_t lastUpdate = 0;
    uint32_t currentTime = lv_tick_get();

    if (currentTime - lastUpdate >= UPDATE_INTERVAL_MS) {
        updateStatus();
        lastUpdate = currentTime;
    }
}

void BluetoothScreen::updateStatus() {
    if (bluetooth_manager.isConnected()) {
        lv_label_set_text(m_statusLabel, "Status: Connected");
        lv_label_set_text(m_connectionLabel, "Ready for communication");
        lv_obj_add_state(m_advertisingSwitch, LV_STATE_CHECKED | LV_STATE_DISABLED);
    } else if (bluetooth_manager.isAdvertising()) {
        lv_label_set_text(m_statusLabel, "Status: Advertising");
        lv_label_set_text(m_connectionLabel, "Scan for 'NanoWatch' on your phone");
        lv_obj_add_state(m_advertisingSwitch, LV_STATE_CHECKED);
        lv_obj_clear_state(m_advertisingSwitch, LV_STATE_DISABLED);
    } else {
        lv_label_set_text(m_statusLabel, "Status: Disconnected");
        lv_label_set_text(m_connectionLabel, "Enable advertising to connect");
        lv_obj_clear_state(m_advertisingSwitch, LV_STATE_CHECKED);
        lv_obj_clear_state(m_advertisingSwitch, LV_STATE_DISABLED);
    }
}

void BluetoothScreen::onAdvertisingSwitchEvent(lv_event_t* event) {
    BluetoothScreen* screen = getLVGLUserData<BluetoothScreen>(lv_event_get_target(event));

    if (lv_obj_has_state(screen->m_advertisingSwitch, LV_STATE_CHECKED)) {
        bluetooth_manager.startAdvertising();
    } else {
        bluetooth_manager.stopAdvertising();
    }

    screen->updateStatus();
}

void BluetoothScreen::onSwipeLeft() {
    // Handle swipe left if needed
}

void BluetoothScreen::onSwipeRight() {
    // Handle swipe right if needed
}