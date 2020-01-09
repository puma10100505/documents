#pragma once

#include "PropertyMacros.h"

namespace yinpsoft
{
class Vector3 final
{
public:
    Vector3() {}
    ~Vector3() {}
    Vector3(float px, float py, float pz)
    {
        set_x(px);
        set_y(py);
        set_z(pz);
    }

public:
    inline static Vector3 Zero()
    {
        return Vector3(0.0f, 0.0f, 0.0f);
    }

private:
    GETSETVAR(float, x, 0.0f);
    GETSETVAR(float, y, 0.0f);
    GETSETVAR(float, z, 0.0f);
};

class Rotation final
{
public:
    Rotation() {}
    ~Rotation() {}
    Rotation(float pp, float py, float pr)
    {
        set_pitch(pp);
        set_yaw(py);
        set_roll(pr);
    }

public:
    inline static Rotation Identity()
    {
        return Rotation(0.0f, 0.0f, 0.0f);
    }

private:
    GETSETVAR(float, pitch, 0.0f);
    GETSETVAR(float, yaw, 0.0f);
    GETSETVAR(float, roll, 0.0f);
};

class Quaternion final
{
public:
    Quaternion() {}
    Quaternion(float x, float y, float z, float w)
    {
        set_x(x);
        set_y(y);
        set_z(z);
        set_w(w);
    }
    ~Quaternion() {}

private:
    GETSETVAR(float, x, 0.0f);
    GETSETVAR(float, y, 0.0f);
    GETSETVAR(float, z, 0.0f);
    GETSETVAR(float, w, 0.0f);
};
}; // namespace yinpsoft