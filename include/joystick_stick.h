#ifndef JOYSTICK_STICK_H
#define JOYSTICK_STICK_H
#include <Arduino.h>

#include <USB.h>
#include <USBHID.h>

#include "joystick.h"

namespace simia
{
// report descriptor
// https://www.usb.org/sites/default/files/documents/hut1_12v2.pdf
char report_descriptor[26] = {
    0x05, 0x01, // USAGE_PAGE (Generic Desktop)
    0x09, 0x04, // USAGE (Joystick)
    0xa1, 0x01, // COLLECTION (Application)
    0xa1, 0x00, //   COLLECTION (Physical)
    0x05, 0x01, //   USAGE_PAGE (Generic Desktop)
    0x09, 0x30, //     USAGE (X)
    0x09, 0x31, //     USAGE (Y)
    0x15, 0xff, //     LOGICAL_MINIMUM (-1)
    0x25, 0x01, //     LOGICAL_MAXIMUM (1)
    0x75, 0x08, //     REPORT_SIZE (8)
    0x95, 0x02, //     REPORT_COUNT (2)
    0x81, 0x02, //     INPUT (Data,Var,Abs)
    0xc0,       //   END_COLLECTION
    0xc0        // END_COLLECTION
};

/// @brief Joystick Stick class
class JoystickStick : USBHIDDevice
{
  private:
    Stick _define;
    USBHID _usbhid;
    bool _is_running{false};

  public:
    JoystickStick(Stick define);
    auto begin() -> void;
    auto start() -> void;
    auto _onGetDescriptor(uint8_t *buffer) -> uint16_t;
    auto send(uint8_t *value) -> bool;
    ~JoystickStick() = default;
};

inline JoystickStick::JoystickStick(Stick define) : _define(define), _usbhid()
{
    pinMode(define.left, INPUT_PULLUP);
    pinMode(define.right, INPUT_PULLUP);
    pinMode(define.up, INPUT_PULLUP);
    pinMode(define.down, INPUT_PULLUP);

    static bool initialized{false};
    if (!initialized)
    {
        initialized = true;
        _usbhid.addDevice(this, sizeof(report_descriptor));
    }
}

inline auto JoystickStick::begin() -> void
{
    _usbhid.begin();
}

inline auto JoystickStick::start() -> void
{
    this->begin();
    _is_running = true;
    uint8_t report[2]{0};
    uint8_t prev_report[2]{0};

    while (_is_running)
    {
        memset(report, 0, sizeof(report));

        if (digitalRead(_define.left) == LOW)
        {
            report[0] = static_cast<uint8_t>(1);
        }
        else if (digitalRead(_define.right) == LOW)
        {
            report[0] = static_cast<uint8_t>(-1);
        }

        if (digitalRead(_define.up) == LOW)
        {
            report[1] = static_cast<uint8_t>(1);
        }
        else if (digitalRead(_define.down) == LOW)
        {
            report[1] = static_cast<uint8_t>(-1);
        }

        if (_usbhid.ready() && memcmp(prev_report, report, sizeof(report)) != 0)
        {
            this->send(report);
            prev_report[0] = report[0];
            prev_report[1] = report[1];
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

// report descriptor for upper computer
inline auto JoystickStick::_onGetDescriptor(uint8_t *buffer) -> uint16_t
{
    memcpy(buffer, report_descriptor, sizeof(report_descriptor));
    return sizeof(report_descriptor);
}

inline auto JoystickStick::send(uint8_t *value) -> bool
{
    return _usbhid.SendReport(0, value, 2);
}

} // namespace simia

#endif // JOYSTICK_STICK_H