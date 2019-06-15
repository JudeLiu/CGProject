//==================================================================================================
// Written in 2016 by Peter Shirley <ptrshrl@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related and
// neighboring rights to this software to the public domain worldwide. This software is distributed
// without any warranty.
//
// You should have received a copy (see file COPYING.txt) of the CC0 Public Domain Dedication along
// with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
//==================================================================================================

#include <iostream>
#include "sphere.h"
#include "hitable_list.h"
#include <float.h>
#include "camera.h"
#include "plane.h"
#include "material.h"
#include <fstream>
#include <typeinfo>
#include "cylinder.h"

int cntPlane = 0, cntSphere = 0, cntObj = 0, cntCy = 0;

vec3 light_source_color(const ray &r, hitable *world, int depth)
{
    hit_record rec;
    if (world->hit(r, 0.001, FLT_MAX, rec))
    {
        ray scattered;
        vec3 attenuation;
        vec3 emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);
        if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered))
            return emitted + attenuation * light_source_color(scattered, world, depth + 1);
        else
            return emitted;
    }
    else
        return vec3(0, 0, 0);
}

vec3 color(const ray &r, hitable *world, int depth)
{
    hit_record rec;
    if (world->hit(r, 1e-3, FLT_MAX, rec))
    {
        vec3 base_color(0, 0, 0);

        if (rec.objT == Plane)
        {
            cntPlane++;
            vec3 q = rec.p;
            if (remainder(10 * q.z(), 6) >= 0 && remainder(10 * q.z(), 6) < 3)
            {
                if (remainder(10 * q.x(), 6) >= 0 && remainder(10 * q.x(), 6) < 3)
                {
                    base_color += 0.5 * vec3(1, 1, 1);
                }
                else
                {
                    base_color += 0.5 * vec3(0, 0, 0);
                }
            }
            else
            {
                if (remainder(10 * q.x(), 6) >= 0 && remainder(10 * q.x(), 6) < 3)
                {

                    base_color += 0.5 * vec3(0, 0, 0);
                }
                else
                {
                    base_color += 0.5 * vec3(1, 1, 1);
                }
            }
        }
        else if (rec.objT == Sphere)
            cntSphere++;
        else if (rec.objT == Cylinder)
            cntCy++;
        else
            cntObj++;

        ray scattered;
        vec3 attenuation;

        if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered))
        {
            // return attenuation * color(scattered, world, depth + 1);
            return base_color + attenuation * color(scattered, world, depth + 1);
        }
        else
        {
            return base_color;
        }
    }
    else
    {
        vec3 unit_direction = unit_vector(r.direction());
        float t = 0.5 * (unit_direction.y() + 1.0);
        return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
    }
}

hitable *one_light_scene()
{
    hitable **list = new hitable *[10];
    int n = 0;
    texture *checker = new checker_texture(new constant_texture(vec3(0.2, 0.3, 0.1)),
                                           new constant_texture(vec3(0.9, 0.9, 0.9)));
    list[n++] = new sphere(vec3(0, 4, 0), .5, new diffuse_light(new constant_texture(vec3(4,4,4))));
    // list[n++] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(new constant_texture(vec3(0.6, 0.4, 0.5))));
    // list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(checker));
    // list[1] = new sphere(vec3(0, 0.5, -1), 0.5, new lambertian(new constant_texture(vec3(0.2, 0.3, 0.5))));
    list[n++] = new plane(vec3(-10, 0, 10), vec3(10, 0, 10), vec3(10, 0, -10), vec3(-10, 0, -10),
                          new metal(vec3(.4, .4, .4), .4));
    // new lambertian(new constant_texture(vec3(1, 1, 1))));
    // list[1] = new sphere(vec3(0, 0.5, -1), 0.5, new lambertian(checker));

    // list[2] = new cylinder(vec3(-1, 0, -1), 0.5, 1, new lambertian(new constant_texture(vec3(0.5, 0.2, 0.4))));
    list[n++] = new cylinder(vec3(2, 0, -1), 0.5, 1.5, new metal(vec3(0.5, .5, .5), .3));
    list[n++] = new sphere(vec3(0, 1, 0), 1, new lambertian(checker));
    list[n++] = new sphere(vec3(4, 2, -1), 1, new diffuse_light(new constant_texture(vec3(4,4,4))));
    // hitable *world = new hitable_list(list,5);
    return new hitable_list(list, n);
}

/* 
240p: 240*320
360p: 360*480
480p: 480*640
720p: 720*1280
1080p: 1080*1920
4k: 2160*3860
*/

int main()
{
    int nx = 480;
    int ny = 360;
    int ns = 500;
    std::ofstream out("cylinder.ppm");
    out << "P3\n"
        << nx << " " << ny << "\n255\n";
    float R = cos(M_PI / 4);

    hitable *world = one_light_scene();

    vec3 lookfrom(3, 10, 10);
    vec3 lookat(0, 0.7, -2);
    float dist_to_focus = 10;
    float aperture = 0;

    camera cam(lookfrom, lookat, vec3(0, 1, 0), 20, float(nx) / float(ny), aperture, dist_to_focus);

    for (int j = ny - 1; j >= 0; j--)
    {
        for (int i = 0; i < nx; i++)
        {
            vec3 col(0, 0, 0);
            for (int s = 0; s < ns; s++)
            {
                float u = float(i + drand48()) / float(nx);
                float v = float(j + drand48()) / float(ny);
                ray r = cam.get_ray(u, v);
                vec3 p = r.point_at_parameter(2.0);
                col += light_source_color(r, world, 0);
            }
            col /= float(ns);
            col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
            int ir = int(255.99 * col[0]);
            int ig = int(255.99 * col[1]);
            int ib = int(255.99 * col[2]);
            out << ir << " " << ig << " " << ib << "\n";
        }
        if (j % 10 == 0)
            std::cout << j << std::endl;
    }
    std::cout << cntPlane << "\t" << cntSphere << '\t' << cntCy << '\t' << cntObj << std::endl;
    out.close();
}
