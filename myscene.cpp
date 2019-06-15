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
#include "cylinder.h"
#include "sphere.h"
#include "hitable_list.h"
#include <float.h>
#include "camera.h"
#include "material.h"
#include "texture.h"
#include "aarect.h"
#include <fstream>

vec3 color(const ray &r, hitable *world, int depth)
{
    hit_record rec;
    if (world->hit(r, 0.001, FLT_MAX, rec))
    {
        ray scattered;
        vec3 attenuation;
        if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered))
        {
            return attenuation * color(scattered, world, depth + 1);
        }
        else
        {
            return vec3(0, 0, 0);
        }
    }
    else
    {
        vec3 unit_direction = unit_vector(r.direction());
        float t = 0.5 * (unit_direction.y() + 1.0);
        return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
    }
}

// hitable *scene()
// {
//     int n = 500;
//     hitable **list = new hitable *[n + 1];
//     list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(vec3(0.5, 0.5, 0.5)));
//     int i = 1;
//     for (int a = -11; a < 11; a++)
//     {
//         for (int b = -11; b < 11; b++)
//         {
//             float choose_mat = drand48();
//             vec3 center(a + 0.9 * drand48(), 0.2, b + 0.9 * drand48());
//             if ((center - vec3(4, 0.2, 0)).length() > 0.9)
//             {
//                 if (choose_mat < 0.8)
//                 { // diffuse
//                     texture *text = new constant_texture(vec3(drand48() * drand48(), drand48() * drand48(), drand48() * drand48()));
//                     list[i++] = new sphere(center, 0.2, new lambertian(text));
//                 }
//                 else if (choose_mat < 0.95)
//                 { // metal
//                     list[i++] = new sphere(center, 0.2,
//                                            new metal(vec3(0.5 * (1 + drand48()), 0.5 * (1 + drand48()), 0.5 * (1 + drand48())), 0.5 * drand48()));
//                 }
//                 else
//                 { // glass
//                     list[i++] = new sphere(center, 0.2, new dielectric(1.5));
//                 }
//             }
//         }
//     }

//     list[i++] = new sphere(vec3(0, 1, 0), 1.0, new dielectric(1.5));
//     // list[i++] = new sphere(vec3(-4, 1, 0), 1.0, new lambertian(vec3(0.4, 0.2, 0.1)));
//     list[i++] = new sphere(vec3(4, 1, 0), 1.0, new metal(vec3(0.7, 0.6, 0.5), 0.0));

//     return new hitable_list(list, i);
// }

hitable *plane_scene()
{
    hitable **list = new hitable *[10];
    int i = 0;
    material *mat1 = new lambertian(new constant_texture(vec3(.5, .4, .3)));
    material *mat2 = new lambertian(new constant_texture(vec3(.3, .6, .5)));
    material *checker = new lambertian(new checker_texture(new constant_texture(vec3(0.05, .05, .05)), new constant_texture(vec3(.95, .95, .95))));
    material *mirror = new metal(.9, .05);
    material *water = new dielectric(2);
    // list[i++] = new sphere(vec3(0,-1000,0), 1000, mat2);
    list[i++] = new xz_rect(-5, 5, -5, 5, 0, checker);
    list[i++] = new cylinder(vec3(1, 0, 0), .3, 1, mat2);
    list[i++] = new cylinder(vec3(0, 0, -1), .2, 1, mat1);
    list[i++] = new sphere(vec3(0, .5, 0), .5, water);
    // list[i++] = new xy_rect(-10, 10, 0, 10, -5, new metal(.95, .02));
    return new hitable_list(list, i);
}

int main()
{
    int nx = 480;
    int ny = 360;
    int ns = 50;
    std::ofstream out("fig.ppm");
    out << "P3\n"
        << nx << " " << ny << "\n255\n";

    float R = cos(M_PI / 4);
    hitable *world = plane_scene();

    vec3 lookfrom(.5, 2, 5);
    vec3 lookat(0, 0, -5);
    float dist_to_focus = 10.0;
    float aperture = 0.;

    camera cam(lookfrom, lookat, vec3(0, 1, 0), 30, float(nx) / float(ny), aperture, dist_to_focus);

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
                col += color(r, world, 0);
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
    out.close();
}
