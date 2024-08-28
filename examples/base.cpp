#include <Arduino.h>
#include <USB.h>

#include <cstring>

#include "joystick.h"

struct DataFrame {
  byte header1{0xAB};
  byte header2{0xCD};
  byte data;
};

USBCDC USBSerial{};

simia::Stick define{.left = 1, .right = 2, .up = 3, .down = 4};

simia::JoystickSerial joystick{define};

void setup() {
  Serial.begin(9600);

  USB.begin();
  USBSerial.begin();
}

void loop() {
  auto result{joystick.read()};
  if (result != simia::DIRECTION::CENTER) {
    DataFrame data_frame{};
    byte buf[sizeof(data_frame)]{};
    data_frame.data = result;
    std::memcpy(buf, &data_frame, sizeof(data_frame));
    USBSerial.write(buf, sizeof(buf));
  }

  delay(10);
}