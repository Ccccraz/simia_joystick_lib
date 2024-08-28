#ifndef JOYSTICK_GAMEPAD_H
#define JOYSTICK_GAMEPAD_H

#include <Arduino.h>

#include <USB.h>
#include <USBHIDGamepad.h>

#include "joystick.h"

namespace simia
{
class JoystickGamepad
{
  private:
    Stick _define{};
    USBHIDGamepad _gamepad{};
    bool is_running = false;

    // default direction is center
    uint8_t _direction{HAT_CENTER};

    // direction map
    // row is X axis
    // column is Y axis
    const uint8_t _direction_map[3][4] = {{HAT_CENTER, HAT_UP, HAT_DOWN},
                                          {HAT_LEFT, HAT_UP_LEFT, HAT_DOWN_LEFT},
                                          {HAT_RIGHT, HAT_UP_RIGHT, HAT_DOWN_RIGHT}};

    auto begin() -> void;

  public:
    JoystickGamepad(Stick define);
    auto start() -> void;
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
}

/// @brief start the joystick gamepad task
/// @return 
inline auto JoystickGamepad::start() -> void
{
    this->begin();
    is_running = true;

    // direction index
    auto row_index = 0;
    auto column_index = 0;

    // previous direction is used to detect changes
    auto _direction_previous = _direction;

    while (is_running)
    {
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
            _gamepad.hat(_direction);
        }

        // delay shortly
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

inline auto JoystickGamepad::stop() -> void
{
    is_running = false;
    _gamepad.end();
}

} // namespace simia

#endif // JOYSTICK_GAMEPAD_H