#pragma once
#include "ui/Screen.h"
#include "bluetooth/BluetoothManager.h"

class BluetoothScreen : public Screen {
public:
    void setup() override;

    void update() override;

    void onSwipeLeft() override;

    void onSwipeRight() override;

private:
    lv_obj_t *m_statusLabel;
    lv_obj_t *m_connectionLabel;
    lv_obj_t *m_advertisingSwitch;
    lv_obj_t *m_pinLabel;

    static constexpr uint32_t UPDATE_INTERVAL_MS = 1000;

    void updateStatus();

    static void onAdvertisingSwitchEvent(lv_event_t *event);
};

template<typename T>
T *getLVGLUserData(lv_obj_t *obj) {
    return static_cast<T *>(lv_obj_get_user_data(obj));
}
