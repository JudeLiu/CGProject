#ifndef TEXTUREH
#define TEXTUREH

#include "vec3.h"

class texture
{
public:
    virtual vec3 value(float u, float v, const vec3 &p) const = 0;
};

class constant_texture : public texture
{
public:
    vec3 color;

    constant_texture() {}
    constant_texture(const vec3 &c) : color(c) {}

    virtual vec3 value(float u, float v, const vec3 &p) const
    {
        return color;
    }
};

class checker_texture : public texture
{
public:
    texture *odd;
    texture *even;

    checker_texture() {}
    checker_texture(texture *t0, texture *t1) : even(t0), odd(t1) {}

    virtual vec3 value(float u, float v, const vec3 &p) const
    {
        // float sines = sin(10 * p.x()) * sin(10 * p.y()) * sin(10 * p.z());
        // if (sines < 0)
        //     return odd->value(u, v, p);
        // else
        //     return even->value(u, v, p);
        if (remainder(10 * p.z(), 6) >= 0 && remainder(10 * p.z(), 6) < 3)
        {
            if (remainder(10 * p.x(), 6) >= 0 && remainder(10 * p.x(), 6) < 3)
            {
                return 0.5 * odd->value(0, 0, p);
            }
            else
            {
                return 0.5 * even->value(0, 0, p);
            }
        }
        else
        {
            if (remainder(10 * p.x(), 6) >= 0 && remainder(10 * p.x(), 6) < 3)
            {

                return 0.5 * even->value(0, 0, p);
            }
            else
            {
                return 0.5 * odd->value(0, 0, p);
            }
        }
    }
};

#endif