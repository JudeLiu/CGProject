#ifndef PLANEH
#define PLANEH

#include <cmath>
#include "hitable.h"
#include <iostream>

// a finite plane
class plane : public hitable
{
private:
    vec3 _norm;

public:
    vec3 a, b, c, d; // vector<a, b> and vector <c, d> are sufficient to define a plane
    material *mat_ptr;

    plane() {}
    plane(vec3 _a, vec3 _b, vec3 _c, vec3 _d, material *m) : a(_a), b(_b), c(_c), d(_d)
    {
        mat_ptr = m;
        vec3 n = unit_vector(cross(b - a, c - b));
        if (dot(n, vec3(0, 1, 0)) > 0)
            _norm = n;
        else
            _norm = -n;
    }

    virtual vec3 norm(const vec3 &p) const { return _norm; }
    bool isInside(const vec3 &p) const;
    virtual bool hit(const ray &r, float tmin, float tmax, hit_record &rec) const;
};

bool plane::isInside(const vec3 &p) const
{
    if (std::abs(dot(p, _norm)) > 1e-4)
    {
        // not on the (infinite) plane
        // std::cout<<"not on the plane\n";
        return false;
    }

    vec3 base1 = b - a, base2 = d - a;
    // (b, base1) <=  (p, base1) <=  (d, base1) and (b, base2) <= (p, base2) <= (d, base2)
    float aDbase1 = dot(a, base1),
          pDbase1 = dot(p, base1),
          bDbase1 = dot(b, base1),
          aDbase2 = dot(a, base2),
          pDbase2 = dot(p, base2),
          dDbase2 = dot(d, base2);
    if( (aDbase1 <= pDbase1 && pDbase1 <= bDbase1) && (aDbase2 <= pDbase2 && pDbase2 <= dDbase2) )
        return true;
    else
        return false;

    // vec3 va = b - a, vb = c - b, vc = d - c, vd = a - d;
    // vec3 ua = p - a, ub = p - b, uc = p - c, ud = p - d;

    // vec3 uaXva = cross(ua, va);
    // vec3 ubXvb = cross(ub, vb);
    // vec3 ucXvc = cross(uc, vc);
    // vec3 udXvd = cross(ud, vd);

    // return dot(uaXva, _norm) >= 0 && dot(ubXvb, _norm) >= 0 &&
    //        dot(ucXvc, _norm) >= 0 && dot(udXvd, _norm) >= 0;
}

bool plane::hit(const ray &r, float tmin, float tmax, hit_record &rec) const
{
    float denom = dot(r.direction(), _norm);
    if (std::abs(denom) < 1e-4)
        return false;

    float t = dot(a - r.origin(), _norm) / denom;
    if (tmin < t && t < tmax)
    {
        vec3 p = r.point_at_parameter(t);
        if (!isInside(p))
        {
            // std::cerr << "outofbound\n";
            return false;
        }
        rec.t = t;
        rec.p = p;
        rec.objT = Plane;
        rec.normal = _norm;
        rec.mat_ptr = mat_ptr;
        return true;
    }
    else
    {
        return false;
    }
}

#endif