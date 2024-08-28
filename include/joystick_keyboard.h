#ifndef JOYSTICK_KEYBOARD_H
#define JOYSTICK_KEYBOARD_H
#include <Arduino.h>

#include <USB.h>
#include <USBHIDKeyboard.h>

#include "joystick.h"

namespace simia
{
/// @brief Which key to simulate
struct KeyboardDefine
{
    uint8_t left;
    uint8_t right;
    uint8_t up;
    uint8_t down;
};

/// @brief Define a joystick to simulate a keyboard
class JoystickKeyboard
{
  private:
    Stick _define{};
    KeyboardDefine _keyboard_define{};
    USBHIDKeyboard _keyboard{};
    bool _is_running{false};

    // previous state of the joystick buttons
    bool _is_left{false};
    bool _is_right{false};
    bool _is_up{false};
    bool _is_down{false};

    auto begin() -> void;

  public:
    JoystickKeyboard(Stick define,
                     KeyboardDefine keyboard_define = {KEY_LEFT_ARROW, KEY_RIGHT_ARROW, KEY_UP_ARROW, KEY_DOWN_ARROW});
    auto start() -> void;
    auto stop() -> void;
    ~JoystickKeyboard() = default;
};

/// @brief Start the joystick keyboard task
/// @return
inline auto JoystickKeyboard::begin() -> void
{
    _keyboard.begin();
}

/// @brief Initialize the joystick keyboard
/// @param define Physical joystick define
/// @param keyboard_define Stimulating keyboard define, default is arrow keys
inline JoystickKeyboard::JoystickKeyboard(Stick define, KeyboardDefine keyboard_define)
    : _define{define}, _keyboard_define{keyboard_define}
{
    pinMode(_define.left, INPUT_PULLUP);
    pinMode(_define.right, INPUT_PULLUP);
    pinMode(_define.up, INPUT_PULLUP);
    pinMode(_define.down, INPUT_PULLUP);
}

/// @brief start the joystick keyboard task
/// @return
inline auto JoystickKeyboard::start() -> void
{
    // Initialize the USB HID keyboard
    this->begin();

    // Start the task
    _is_running = true;
    while (_is_running)
    {
        // Read the state of the joystick buttons
        auto left = digitalRead(_define.left);
        auto right = digitalRead(_define.right);
        auto up = digitalRead(_define.up);
        auto down = digitalRead(_define.down);

        // Detect if the left button is pressed
        if (left == LOW)
        {
            _keyboard.press(_keyboard_define.left);
            _is_left = true;
        }
        else if (_is_left)
        {
            _keyboard.release(_keyboard_define.left);
            _is_left = false;
        }

        // Detect if the right button is pressed
        if (right == LOW)
        {
            _keyboard.press(_keyboard_define.right);
            _is_right = true;
        }
        else if (_is_right)
        {
            _keyboard.release(_keyboard_define.right);
            _is_right = false;
        }

        // Detect if the up button is pressed
        if (up == LOW)
        {
            _keyboard.press(_keyboard_define.up);
            _is_up = true;
        }
        else if (_is_up)
        {
            _keyboard.release(_keyboard_define.up);
            _is_up = false;
        }

        // Detect if the down button is pressed
        if (down == LOW)
        {
            _keyboard.press(_keyboard_define.down);
            _is_down = true;
        }
        else if (_is_down)
        {
            _keyboard.release(_keyboard_define.down);
            _is_down = false;
        }

        // Delay for a short period of time
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

/// @brief Stop the joystick keyboard task
/// @return
inline auto JoystickKeyboard::stop() -> void
{
    _is_running = false;
    _keyboard.end();
}
} // namespace simia

#endif