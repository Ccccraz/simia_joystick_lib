#ifndef JOYSTICK_H
#define JOYSTICK_H

namespace simia
{
enum DIRECTION
{
    LEFT,
    RIGHT,
    UP,
    DOWN,
    CENTER
};

struct Stick
{
    int left;
    int right;
    int up;
    int down;
};
} // namespace simia

#endif // JOYSTICK_H