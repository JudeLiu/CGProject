#ifndef CYLINDERH
#define CYLINDERH

#include "object.h"
#include <cmath>

// a simple cylinder perpendicular to xoz plane
class cylinder : public object
{
public:
    vec3 center;
    float radius;
    float height;
    material *mat_ptr;

    cylinder() {}
    cylinder(const vec3 &c, float r, float h, material *m) : center(c)
    {
        radius = r;
        height = h;
        mat_ptr = m;
    }

    vec3 norm(const vec3 &p) const;

    virtual bool bounding_box(float t0, float t1, aabb &box) const;
    virtual bool hit(const ray &r, float tmin, float tmax, hit_record &rec) const;
};

vec3 cylinder::norm(const vec3 &p) const
{
    if (std::abs(p.z() - height) < 1e-3)
    {
        // hit the top
        return vec3(0, 1, 0);
    }
    if (std::abs(p.z() < 1e-3))
    {
        // hit the bottom
        return vec3(0, -1, 0);
    }

    vec3 c(center);
    c[1] = p[1];
    return unit_vector(p - c);
    // vec3 n = p - center;
    // n[1] = 0;
    // return unit_vector(n);
}

bool cylinder::bounding_box(float t0, float t1, aabb &box) const
{
    vec3 _min(center.x() - radius, center.y(), center.z() - radius);
    vec3 _max(center.x() + radius, center.y() + height, center.z() + radius);
    box = aabb(_min, _max);
    return true;
}

bool cylinder::hit(const ray &r, float tmin, float tmax, hit_record &rec) const
{
    // the coefficients are cancelled, in b, discriminant and equation of the root

    // virtual direction, set y=0
    float a = pow(r.direction().x(), 2) + pow(r.direction().z(), 2);
    float b = r.direction().x() * (r.origin().x() - center.x()) +
              r.direction().z() * (r.origin().z() - center.z());
    float c = pow((r.origin().x() - center.x()), 2) + pow(r.origin().z() - center.z(), 2) - radius * radius;
    float discriminant = b * b - a * c;

    bool isHit = false;
    float t;
    if (discriminant > 0)
    {
        float t1 = (-b - sqrt(discriminant)) / a;
        float t2 = (-b + sqrt(discriminant)) / a;
        if (t1 > t2)
            std::swap(t1, t2);

        float y1 = (r.origin().y() + t1 * r.direction().y());
        float y2 = (r.origin().y() + t2 * r.direction().y());

        float ymin = center.y();
        if (y1 < ymin)
        {
            if (y2 < ymin)
                isHit = false;
            else
            {
                // hit the bottom
                float th = t1 + (t2 - t1) * ((y1 - ymin) / (y1 - y2));

                if (!(tmin < th && th < tmax))
                    isHit = false;
                else
                {
                    isHit = true;
                    t = th;
                    // std::cerr << "bottom\n";
                }
            }
        }
        else if (ymin <= y1 && y1 <= height)
        {
            if (!(tmin < t1 && t1 < tmax))
                isHit = false;
            else
            {
                // hit the side
                t = t1;
                isHit = true;
                // std::cerr << "side\n";
            }
        }
        else if (y1 > height)
        {
            if (y2 > height)
                isHit = false;
            else
            {
                float th = t1 + (t2 - t1) * ((y1 - height) / (y1 - y2));
                if (!(tmin < th && th < tmax))
                    isHit = false;
                else
                {
                    // hit the top
                    t = th;
                    isHit = true;
                    // std::cerr << "top\n";
                }
            }
        }
    }

    if (isHit)
    {
        rec.objT = Cylinder;
        rec.mat_ptr = mat_ptr;
        rec.t = t;
        rec.p = r.point_at_parameter(t);
        rec.normal = norm(rec.p);
    }

    return isHit;
}

#endif