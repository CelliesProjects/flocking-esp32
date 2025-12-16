#pragma once
#include <math.h>

struct Vec2
{
    float x;
    float y;

    // Constructors
    Vec2() : x(0), y(0) {}
    Vec2(float x_, float y_) : x(x_), y(y_) {}

    // ----- Basic operations -----
    inline Vec2 operator+(const Vec2 &v) const { return Vec2(x + v.x, y + v.y); }
    inline Vec2 operator-(const Vec2 &v) const { return Vec2(x - v.x, y - v.y); }
    inline Vec2 operator*(float s) const { return Vec2(x * s, y * s); }
    inline Vec2 operator/(float s) const { return Vec2(x / s, y / s); }

    inline Vec2 &operator+=(const Vec2 &v)
    {
        x += v.x;
        y += v.y;
        return *this;
    }
    inline Vec2 &operator-=(const Vec2 &v)
    {
        x -= v.x;
        y -= v.y;
        return *this;
    }
    inline Vec2 &operator*=(float s)
    {
        x *= s;
        y *= s;
        return *this;
    }
    inline Vec2 &operator/=(float s)
    {
        x /= s;
        y /= s;
        return *this;
    }

    // ----- Magnitude -----
    inline float mag() const { return sqrtf(x * x + y * y); }
    inline float magSq() const { return x * x + y * y; } // no sqrt (faster)

    // ----- Normalize -----
    inline void normalize()
    {
        float m = mag();
        if (m > 0.00001f)
        {
            x /= m;
            y /= m;
        }
    }

    inline Vec2 normalized() const
    {
        float m = mag();
        return (m > 0.00001f) ? Vec2(x / m, y / m) : Vec2(0, 0);
    }

    // ----- Limit vector length -----
    inline void limit(float max)
    {
        float mSq = magSq();
        if (mSq > max * max)
        {
            float m = sqrtf(mSq);
            x = x / m * max;
            y = y / m * max;
        }
    }

    // ----- Distance functions -----
    inline float dist(const Vec2 &v) const
    {
        float dx = x - v.x;
        float dy = y - v.y;
        return sqrtf(dx * dx + dy * dy);
    }

    inline float distSq(const Vec2 &v) const
    {
        float dx = x - v.x;
        float dy = y - v.y;
        return dx * dx + dy * dy;
    }

    // ----- Angle (for rendering boids) -----
    inline float heading() const { return atan2f(y, x); }

    // ----- Static helpers -----
    static inline Vec2 sub(const Vec2 &a, const Vec2 &b)
    {
        return Vec2(a.x - b.x, a.y - b.y);
    }

    static inline Vec2 add(const Vec2 &a, const Vec2 &b)
    {
        return Vec2(a.x + b.x, a.y + b.y);
    }
};
