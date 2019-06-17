#ifndef RAYH
#define RAYH
#include "vec3.h"

class ray
{
    vec3 _origin, _dir;

public:
    ray() {}
    ray(const vec3 &o, const vec3 &d) : _origin(o), _dir(d)
    {
    }

    vec3 origin() const { return _origin; }
    vec3 direction() const { return _dir; }
    vec3 point_at_parameter(float t) const { return _origin + t * _dir; }
};

#endif