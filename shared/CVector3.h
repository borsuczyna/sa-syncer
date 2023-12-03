#pragma once
#include <corecrt_math.h>

class CVector3
{
public:
    float x, y, z;

    CVector3()
    {
        x = y = z = 0.0f;
    }

    CVector3(float x, float y, float z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    float Distance(CVector3 vec)
    {
        return sqrtf(powf(vec.x - x, 2) + powf(vec.y - y, 2) + powf(vec.z - z, 2));
    }

    float Distance(float x, float y, float z)
    {
        return sqrtf(powf(x - this->x, 2) + powf(y - this->y, 2) + powf(z - this->z, 2));
    }

    float Distance2D(CVector3 vec)
    {
        return sqrtf(powf(vec.x - x, 2) + powf(vec.y - y, 2));
    }

    float Distance2D(float x, float y)
    {
        return sqrtf(powf(x - this->x, 2) + powf(y - this->y, 2));
    }

    float Length()
    {
        return sqrtf(powf(x, 2) + powf(y, 2) + powf(z, 2));
    }

    float Length2D()
    {
        return sqrtf(powf(x, 2) + powf(y, 2));
    }

    CVector3 Normalize()
    {
        float len = Length();
        if (len == 0.0f)
            return CVector3(0.0f, 0.0f, 0.0f);
        return CVector3(x / len, y / len, z / len);
    }

    CVector3 operator+(CVector3 vec)
    {
        return CVector3(x + vec.x, y + vec.y, z + vec.z);
    }

    CVector3 operator-(CVector3 vec)
    {
        return CVector3(x - vec.x, y - vec.y, z - vec.z);
    }

    CVector3 operator*(CVector3 vec)
    {
        return CVector3(x * vec.x, y * vec.y, z * vec.z);
    }

    CVector3 operator/(CVector3 vec)
    {
        return CVector3(x / vec.x, y / vec.y, z / vec.z);
    }

    CVector3 operator+(float val)
    {
        return CVector3(x + val, y + val, z + val);
    }

    CVector3 operator-(float val)
    {
        return CVector3(x - val, y - val, z - val);
    }

    CVector3 operator*(float val)
    {
        return CVector3(x * val, y * val, z * val);
    }

    CVector3 operator/(float val)
    {
        return CVector3(x / val, y / val, z / val);
    }

    CVector3 operator+=(CVector3 vec)
    {
        x += vec.x;
        y += vec.y;
        z += vec.z;
        return *this;
    }

    CVector3 operator-=(CVector3 vec)
    {
        x -= vec.x;
        y -= vec.y;
        z -= vec.z;
        return *this;
    }

    CVector3 operator*=(CVector3 vec)
    {
        x *= vec.x;
        y *= vec.y;
        z *= vec.z;
        return *this;
    }

    CVector3 operator/=(CVector3 vec)
    {
        x /= vec.x;
        y /= vec.y;
        z /= vec.z;
        return *this;
    }

    CVector3 operator+=(float val)
    {
        x += val;
        y += val;
        z += val;
        return *this;
    }

    CVector3 operator-=(float val)
    {
        x -= val;
        y -= val;
        z -= val;
        return *this;
    }

    CVector3 operator*=(float val)
    {
        x *= val;
        y *= val;
        z *= val;
        return *this;
    }

    CVector3 operator/=(float val)
    {
        x /= val;
        y /= val;
        z /= val;
        return *this;
    }

    bool operator==(CVector3 vec)
    {
        return (x == vec.x && y == vec.y && z == vec.z);
    }

    bool operator!=(CVector3 vec)
    {
        return (x != vec.x || y != vec.y || z != vec.z);
    }

    bool operator==(float val)
    {
        return (x == val && y == val && z == val);
    }

    bool operator!=(float val)
    {
        return (x != val || y != val || z != val);
    }

    bool operator>(CVector3 vec)
    {
        return (x > vec.x && y > vec.y && z > vec.z);
    }

    bool operator<(CVector3 vec)
    {
        return (x < vec.x && y < vec.y && z < vec.z);
    }

    bool operator>(float val)
    {
        return (x > val && y > val && z > val);
    }

    bool operator<(float val)
    {
        return (x < val && y < val && z < val);
    }

    bool operator>=(CVector3 vec)
    {
        return (x >= vec.x && y >= vec.y && z >= vec.z);
    }

    bool operator<=(CVector3 vec)
    {
        return (x <= vec.x && y <= vec.y && z <= vec.z);
    }

    bool operator>=(float val)
    {
        return (x >= val && y >= val && z >= val);
    }

    bool operator<=(float val)
    {
        return (x <= val && y <= val && z <= val);
    }
};