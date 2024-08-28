#ifndef JOYSTICK_SERIAL_H
#define JOYSTICK_SERIAL_H
#include "joystick.h"
#include <Arduino.h>

namespace simia
{

class JoystickSerial
{
  private:
    Stick _define{};

  public:
    JoystickSerial(Stick define);
    auto read() -> DIRECTION;
    ~JoystickSerial() = default;
};

inline JoystickSerial::JoystickSerial(Stick define) : _define(define)
{
    pinMode(define.left, INPUT_PULLUP);
    pinMode(define.right, INPUT_PULLUP);
    pinMode(define.up, INPUT_PULLUP);
    pinMode(define.down, INPUT_PULLUP);
}

inline auto JoystickSerial::read() -> DIRECTION
{
    if (digitalRead(_define.left) == LOW)
    {
        return DIRECTION::LEFT;
    }
    else if (digitalRead(_define.right) == LOW)
    {
        return DIRECTION::RIGHT;
    }
    else if (digitalRead(_define.up) == LOW)
    {
        return DIRECTION::UP;
    }
    else if (digitalRead(_define.down) == LOW)
    {
        return DIRECTION::DOWN;
    }
    return DIRECTION::CENTER;
}
} // namespace simia

#endif