#include <Arduino.h>
#include <USB.h>

#include <cstring>

#include "joystick.h"
#include "joystick_hid.h"

simia::Stick define{.left = 1, .right = 2, .up = 3, .down = 4};

simia::JoystickStick joystick{define};

void setup()
{
    USB.begin();
    joystick.start();
}

void loop()
{
}