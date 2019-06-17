#ifndef SPHEREH
#define SPHEREH

#include "object.h"

class sphere : public object
{
public:
    vec3 center;
    float radius;
    material *mat_ptr;

    sphere() {}
    sphere(vec3 cen, float r) : center(cen), radius(r) {}
    sphere(vec3 cen, float r, material *m) : center(cen)
    {
        radius = r;
        mat_ptr = m;
    }

    vec3 norm(const vec3 &p) const
    {
        return center - p;
    }
    virtual bool bounding_box(float t0, float t1, aabb& box) const;
    virtual bool hit(const ray &r, float tmin, float tmax, hit_record &rec) const;
};

bool sphere::bounding_box(float t0, float t1, aabb& box) const {
    box = aabb(center - vec3(radius, radius, radius), center + vec3(radius, radius, radius));
    return true;
}

bool sphere::hit(const ray &r, float t_min, float t_max, hit_record &rec) const
{
    vec3 oc = r.origin() - center;
    // the coefficients are cancelled, in b, discriminant and equation of the root
    float a = dot(r.direction(), r.direction());
    float b = dot(r.direction(), oc);
    float c = dot(oc, oc) - radius * radius;
    float discriminant = b * b - a * c;

    if (discriminant > 0)
    {
        // float temp = (-b - sqrt(discriminant)) / a;
        float t1 = (-b - sqrt(discriminant)) / a;
        float t2 = (-b + sqrt(discriminant)) / a;
        float t;

        if (t1 > t2)
            std::swap(t1, t2);

        if (t1 < t_max && t1 > t_min)
            t = t1;
        else if (t_min < t2 && t2 < t_max)
            t = t2;
        else 
            return false;
        
        rec.t = t;
        rec.objT = Sphere;
        rec.p = r.point_at_parameter(t1);
        get_sphere_uv((rec.p - center) / radius, rec.u, rec.v);
        rec.normal = (rec.p - center) / radius;
        rec.mat_ptr = mat_ptr;
        return true;
    }

    return false;
}

#endif