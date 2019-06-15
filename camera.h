#ifndef CAMERAH
#define CAMERAH

#define CAMERAV4

#include "ray.h"

#ifdef CAMERAV1
class camera
{
public:
    vec3 origin;
    vec3 lower_left_corner;
    vec3 horizontal;
    vec3 vertical;

    camera() : lower_left_corner(vec3(-2.0, -1.0, -1.0)),
               horizontal(vec3(4.0, 0.0, 0.0)),
               vertical(vec3(0.0, 2.0, 0.0)),
               origin(vec3(0, 0, 0))
    {
    }

    ray get_ray(float u, float v)
    {
        return ray(origin, lower_left_corner + u * horizontal + v * vertical - origin);
    }
};
#endif

#ifdef CAMERAV2
class camera
{
public:
    vec3 origin;
    vec3 lower_left_corner;
    vec3 horizontal;
    vec3 vertical;

    camera(float vfov, float aspect)
    {
        float theta = vfov * M_PI / 180;
        float half_height = tan(theta / 2);
        float half_width = aspect * half_height;
        lower_left_corner = vec3(-half_width, -half_height, -1.0);
        horizontal = vec3(2 * half_width, 0.0, 0.0);
        vertical = vec3(0.0, 2 * half_height, 0.0);
        origin = vec3(0.0, 0.0, 0.0);
    }
    ray get_ray(float u, float v)
    {
        return ray(origin, lower_left_corner + u * horizontal + v * vertical - origin);
    }
};
#endif

#ifdef CAMERAV3
class camera
{
public:
    vec3 origin;
    vec3 lower_left_corner;
    vec3 horizontal;
    vec3 vertical;

    camera(vec3 lookfrom, vec3 lookat, vec3 vup, float vfov, float aspect)
    {
        // vfov is top to bottom in degree
        vec3 u, v, w;
        float theta = vfov * M_PI / 180;
        float half_height = tan(theta / 2);
        float half_width = aspect * half_height;
        origin = lookfrom;
        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup, w));
        v = cross(w, u);
        // lower_left_corner = vec3(-half_width, -half_height, -1.0);
        lower_left_corner = origin - half_width * u - half_height * v - w;
        horizontal = 2 * half_width * u;
        vertical = 2 * half_width * v;
    }

    ray get_ray(float u, float v)
    {
        return ray(origin, lower_left_corner + u * horizontal + v * vertical - origin);
    }
};
#endif

#ifdef CAMERAV4
vec3 random_in_unit_disk()
{
    vec3 p;
    do
    {
        p = 2.0 * vec3(drand48(), drand48(), 0) - vec3(1, 1, 0);
    } while (dot(p, p) >= 1.0);

    return p;
}

class camera
{
private:
    vec3 origin;
    vec3 lower_left_corner;
    vec3 horizontal;
    vec3 vertical;
    vec3 u, v, w;
    float lens_radius;

public:
    // vfov is top to bottom in degree, vertical field of view
    //
    camera(vec3 lookfrom, vec3 lookat, vec3 vup, float vfov, float aspect, float aperture, float focus_dist)
    {
        lens_radius = aperture / 2;
        float theta = vfov * M_PI / 180;
        float half_height = tan(theta / 2);
        float half_width = aspect * half_height;
        origin = lookfrom;
        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup, w));
        v = cross(w, u);
        // lower_left_corner = vec3(-half_width, -half_height, -1.0);
        lower_left_corner = origin - half_width * focus_dist * u - half_height * focus_dist * v - focus_dist * w;
        horizontal = 2 * half_width * focus_dist * u;
        vertical = 2 * half_height * focus_dist * v;
    }

    ray get_ray(float s, float t)
    {
        vec3 rd = lens_radius * random_in_unit_disk();
        vec3 offset = u * rd.x() + v * rd.y();
        return ray(origin + offset, lower_left_corner + s * horizontal + t * vertical - origin - offset);
    }
};
#endif

#endif