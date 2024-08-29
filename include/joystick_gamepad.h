#ifndef JOYSTICK_GAMEPAD_H
#define JOYSTICK_GAMEPAD_H

#include <Arduino.h>

#include <USB.h>
#include <USBHID.h>

#include "joystick.h"

namespace simia
{

#define CENTER 0
#define UP 1
#define DOWN 2
#define LEFT 4
#define UP_LEFT 5
#define DOWN_LEFT 6
#define RIGHT 8
#define UP_RIGHT 9
#define DOWN_RIGHT 10

char gamepad_report_descriptor[24] = {
    0x05, 0x01, // USAGE_PAGE (Generic Desktop)
    0x09, 0x04, // USAGE (Joystick)
    0xa1, 0x01, // COLLECTION (Application)
    0xa1, 0x00, //   COLLECTION (Physical)
    0x05, 0x01, //     USAGE_PAGE (Generic Desktop)
    0x09, 0x39, //     USAGE (Hat switch)
    0x15, 0x01, //     LOGICAL_MINIMUM (1)
    0x25, 0x08, //     LOGICAL_MAXIMUM (10)
    0x75, 0x08, //     REPORT_SIZE (8)
    0x95, 0x01, //     REPORT_COUNT (1)
    0x81, 0x02, //     INPUT (Data,Var,Abs,Null)
    0xc0,       //   END_COLLECTION
    0xc0        // END_COLLECTION
};

class JoystickGamepad : public USBHIDDevice
{
  private:
    Stick _define{};
    USBHID _gamepad{};
    bool is_running{false};

    // default direction is center
    signed char _direction{CENTER};

    // direction map
    // row is X axis
    // column is Y axis
    const signed char _direction_map[3][3] = {
        {CENTER, UP, DOWN}, {LEFT, UP_LEFT, DOWN_LEFT}, {RIGHT, UP_RIGHT, DOWN_RIGHT}};

    auto begin() -> void;

  public:
    JoystickGamepad(Stick define);
    auto start() -> void;
    auto _onGetDescriptor(uint8_t *buffer) -> uint16_t;
    auto stop() -> void;
    ~JoystickGamepad() = default;
};

inline auto JoystickGamepad::begin() -> void
{
    _gamepad.begin();
}

/// @brief Initialize the joystick gamepad
/// @param define  Physical joystick pins
inline JoystickGamepad::JoystickGamepad(Stick define) : _define{define}
{
    pinMode(_define.left, INPUT_PULLUP);
    pinMode(_define.right, INPUT_PULLUP);
    pinMode(_define.up, INPUT_PULLUP);
    pinMode(_define.down, INPUT_PULLUP);

    static bool initialized{false};
    if (!initialized)
    {
        initialized = true;
        _gamepad.addDevice(this, sizeof(gamepad_report_descriptor));
    }
}

/// @brief start the joystick gamepad task
/// @return
inline auto JoystickGamepad::start() -> void
{
    this->begin();
    is_running = true;

    // previous direction is used to detect changes
    auto _direction_previous = _direction;

    while (is_running)
    {
        // direction index
        auto row_index = 0;
        auto column_index = 0;

        auto left = digitalRead(_define.left);
        auto right = digitalRead(_define.right);
        auto up = digitalRead(_define.up);
        auto down = digitalRead(_define.down);

        // direction row index
        if (left == LOW)
        {
            row_index = 1;
        }
        else if (right == LOW)
        {
            row_index = 2;
        }
        else
        {
            row_index = 0;
        }

        // direction column index
        if (up == LOW)
        {
            column_index = 1;
        }
        else if (down == LOW)
        {
            column_index = 2;
        }
        else
        {
            column_index = 0;
        }

        // direction
        _direction = _direction_map[row_index][column_index];

        if (_direction_previous != _direction)
        {
            _gamepad.SendReport(0, &_direction, 1);
            _direction_previous = _direction;
        }

        // delay shortly
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

inline auto JoystickGamepad::_onGetDescriptor(uint8_t *buffer) -> uint16_t
{
    memcpy(buffer, gamepad_report_descriptor, sizeof(gamepad_report_descriptor));
    return sizeof(gamepad_report_descriptor);
}

inline auto JoystickGamepad::stop() -> void
{
    is_running = false;
    _gamepad.end();
}

} // namespace simia

#endif // JOYSTICK_GAMEPAD_H