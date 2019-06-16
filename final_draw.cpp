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
#include <fstream>
#include <typeinfo>
#include <float.h>
#include <random>
#include <ctime>

#include "headers.h"

const int RT_DEPTH = 10;

int cntPlane = 0, cntSphere = 0, cntObj = 0, cntCy = 0, cntRect = 0;

inline int clamp(int n, int minimal = 0, int maximal = 255)
{
    return std::min(std::max(n, minimal), maximal);
}

vec3 light_source_color(const ray &r, hitable *world, int depth)
{
    hit_record rec;
    if (world->hit(r, 0.001, FLT_MAX, rec))
    {
        ray scattered;
        vec3 attenuation;
        vec3 emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);
        if (depth < RT_DEPTH && rec.mat_ptr->scatter(r, rec, attenuation, scattered))
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
            // vec3 q = rec.p;
            // if (remainder(10 * q.z(), 6) >= 0 && remainder(10 * q.z(), 6) < 3)
            // {
            //     if (remainder(10 * q.x(), 6) >= 0 && remainder(10 * q.x(), 6) < 3)
            //     {
            //         base_color += 0.5 * vec3(1, 1, 1);
            //     }
            //     else
            //     {
            //         base_color += 0.5 * vec3(0, 0, 0);
            //     }
            // }
            // else
            // {
            //     if (remainder(10 * q.x(), 6) >= 0 && remainder(10 * q.x(), 6) < 3)
            //     {

            //         base_color += 0.5 * vec3(0, 0, 0);
            //     }
            //     else
            //     {
            //         base_color += 0.5 * vec3(1, 1, 1);
            //     }
            // }
        }
        else if (rec.objT == Sphere)
            cntSphere++;
        else if (rec.objT == Cylinder)
            cntCy++;
        else if (rec.objT == Rectangle)
            cntRect++;
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

hitable *light_scene()
{
    hitable **list = new hitable *[10];
    int n = 0;
    texture *checker = new checker_texture(new constant_texture(vec3(0.2, 0.3, 0.1)),
                                           new constant_texture(vec3(0.9, 0.9, 0.9)));
    list[n++] = new sphere(vec3(0, 4, 0), .5, new diffuse_light(new constant_texture(vec3(4, 4, 4))));
    // list[n++] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(new constant_texture(vec3(0.6, 0.4, 0.5))));
    // list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(checker));
    // list[1] = new sphere(vec3(0, 0.5, -1), 0.5, new lambertian(new constant_texture(vec3(0.2, 0.3, 0.5))));
    // list[n++] = new plane(vec3(-10, 0, 10), vec3(10, 0, 10), vec3(10, 0, -10), vec3(-10, 0, -10),
    //                       new lambertian(checker));
    //   new metal(vec3(.4, .4, .4), .4));
    // new lambertian(new constant_texture(vec3(1, 1, 1))));
    // list[1] = new sphere(vec3(0, 0.5, -1), 0.5, new lambertian(checker));

    // list[2] = new cylinder(vec3(-1, 0, -1), 0.5, 1, new lambertian(new constant_texture(vec3(0.5, 0.2, 0.4))));
    list[n++] = new cylinder(vec3(2, 0, 1), 0.5, 1.5, new metal(vec3(0.5, .5, .5), .3));
    list[n++] = new sphere(vec3(0, 1, 0), 1, new lambertian(checker));
    // list[n++] = new sphere(vec3(4, 2, -1), 1, new diffuse_light(new constant_texture(vec3(4, 4, 4))));
    list[n++] = new yz_rect(0, 2, -0.5, 0.5, -3, new diffuse_light(new constant_texture(vec3(4, 0, 0))));
    list[n++] = new yz_rect(0, 2, -0.5, 0.5, 3, new diffuse_light(new constant_texture(vec3(0, 4, 4))));

    // hitable *world = new hitable_list(list,5);
    return new hitable_list(list, n);
}

hitable *simple_light()
{
    hitable **list = new hitable *[4];
    list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(new constant_texture(vec3(.1, .3, .6))));
    list[1] = new sphere(vec3(0, 2, 0), 2, new lambertian(new constant_texture(vec3(.8, .3, .1))));
    list[2] = new sphere(vec3(0, 7, 0), 2, new diffuse_light(new constant_texture(vec3(4, 4, 4))));
    // list[3] = new xy_rect(3, 5, 1, 3, -2, new diffuse_light(new constant_texture(vec3(4, 4, 4))));
    list[3] = new xy_rect(3, 0, 4, 4, 0, new diffuse_light(new constant_texture(vec3(4, 0, 0))));
    return new hitable_list(list, 4);
}

hitable *sun_scene()
{
    hitable **list = new hitable *[10];
    int n = 0;
    list[n++] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(new constant_texture(vec3(0.1, 0.8, 0.4))));
    list[n++] = new sphere(vec3(0, 0.5, -1), 0.5, new lambertian(new constant_texture(vec3(0.2, 0.3, 0.5))));
    list[n++] = new sphere(vec3(1, .5, -1), .5, new dielectric(1.33));

    list[n++] = new cylinder(vec3(-2, 0, 0), 1, 1, new dielectric(1.33));
    list[n++] = new cylinder(vec3(2, 0, 0), .4, 1, new metal(vec3(1, 0, 0), 0));
    list[n++] = new cylinder(vec3(-2, 0, -4), 0.2, 2, new lambertian(new constant_texture(vec3(.6, .4, .4))));

    return new hitable_list(list, n);
}

hitable *cornell_box()
{
    hitable **list = new hitable *[30];
    int n = 0;
    int ld = 4; // light density
    vec3 cyan(64. / 255., 224. / 255., 208. / 255.);
    vec3 some_yellow(.95, 214.f / 255.f, 51.f / 255.f);

    material *red = new lambertian(new constant_texture(vec3(.65, .05, .05)));
    material *gray = new lambertian(new constant_texture(vec3(.1, .1, .1)));
    material *white = new lambertian(new constant_texture(vec3(.73, .73, .73)));
    material *green = new lambertian(new constant_texture(vec3(.12, .45, .15)));
    material *blue = new lambertian(new constant_texture(vec3(.05, .05, .55)));
    material *light = new diffuse_light(new constant_texture(vec3(ld, ld, ld)));
    material *cyan_light = new diffuse_light(new constant_texture(cyan));
    material *water = new dielectric(1.33);
    material *glass = new dielectric(1.55);
    material *smooth_metal = new metal(vec3(.1, .1, .68), 0.1);
    material *almost_mirror = new metal(0.5, 0.1);
    material *mirror = new metal(.9, 0);

    list[n++] = new flip_normals(new yz_rect(0, 555, 0, 555, 555, green)); // left wall
    list[n++] = new yz_rect(0, 555, 0, 555, 0, red);                       // right
    list[n++] = new xz_rect(113, 443, 127, 432, 554, light);               // ceiling light
    list[n++] = new flip_normals(new xz_rect(0, 555, 0, 555, 555, white)); // ceiling
    list[n++] = new xz_rect(0, 555, 0, 555, 0, gray);                      // floor
    list[n++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, white)); // back

    list[n++] = new yz_rect(10, 200, 100, 340, 1, mirror);       // mirror on the right wall
    list[n++] = new sphere(vec3(60, 50, 330), 50, smooth_metal); // blue metal ball

    list[n++] = new sphere(vec3(250, 165 + 60, 250), 60, glass); // in the middle air
    list[n++] = new translate(new rotate_y(new box(vec3(0, 0, 0), vec3(165, 165, 165), new dielectric(1.5)), -18), vec3(220, 0, 220));
    // list[n++] = new sphere(vec3(400, 250, 250), 10, new diffuse_light(new constant_texture(vec3(500,500,500)))); // nuke light
    // list[n++] = new sphere(vec3(80, 150, 250), 40, light);

    list[n++] = new cylinder(vec3(400, 0, 400), 100, 100, new lambertian(new constant_texture(some_yellow)));
    // list[n++] = new sphere(vec3(400, 240, 240), 50, water);

    // list[n++] = new box(vec3(250, 0, 0), vec3(350, 100, 100), light); //front light
    list[n++] = new sphere(vec3(450, 50, 50), 20, new diffuse_light(new constant_texture(vec3(10, 10, 10))));

    return new hitable_list(list, n);
}

hitable *cornell_glass_focus()
{
    hitable **list = new hitable *[30];
    int n = 0;
    int ld = 4; // light intensity
    vec3 cyan(64. / 255., 224. / 255., 208. / 255.);
    vec3 some_yellow(.95, 214.f / 255.f, 51.f / 255.f);

    material *red = new lambertian(new constant_texture(vec3(.65, .05, .05)));
    material *gray = new lambertian(new constant_texture(vec3(.1, .1, .1)));
    material *white = new lambertian(new constant_texture(vec3(.73, .73, .73)));
    material *green = new lambertian(new constant_texture(vec3(.12, .45, .15)));
    material *blue = new lambertian(new constant_texture(vec3(.05, .05, .55)));
    material *light = new diffuse_light(new constant_texture(vec3(ld, ld, ld)));
    material *cyan_light = new diffuse_light(new constant_texture(cyan));
    material *water = new dielectric(1.33);
    material *glass = new dielectric(1.55);
    material *smooth_metal = new metal(vec3(.1, .1, .68), 0.1);
    material *almost_mirror = new metal(0.5, 0.1);
    material *mirror = new metal(.9, 0);

    list[n++] = new flip_normals(new yz_rect(0, 555, 0, 555, 555, green)); // left wall
    list[n++] = new yz_rect(0, 555, 0, 555, 0, red);                       // right
    list[n++] = new xz_rect(113, 443, 127, 432, 554, light);               // ceiling light
    list[n++] = new flip_normals(new xz_rect(0, 555, 0, 555, 555, white)); // ceiling
    list[n++] = new xz_rect(0, 555, 0, 555, 0, gray);                      // floor
    list[n++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, white)); // back

    // list[n++] = new yz_rect(10, 200, 100, 340, 1, mirror);       // mirror on the right wall
    // list[n++] = new sphere(vec3(60, 50, 330), 50, smooth_metal); // blue metal ball

    list[n++] = new sphere(vec3(250, 500, 250), 10, new diffuse_light(new constant_texture(vec3(100, 100, 100))));
    list[n++] = new sphere(vec3(250, 350, 250), 60, glass); // in the middle air

    // list[n++] = new sphere(vec3(400, 250, 250), 10, new diffuse_light(new constant_texture(vec3(500,500,500)))); // nuke light
    // list[n++] = new sphere(vec3(80, 150, 250), 40, light);

    list[n++] = new cylinder(vec3(400, 0, 400), 100, 100, new lambertian(new constant_texture(some_yellow)));

    return new hitable_list(list, n);
}

camera cornel_camera(int nx, int ny)
{
    vec3 lookfrom(278, 278, -800);
    vec3 lookat(278, 278, 0);
    float dist_to_focus = 10;
    float aperture = 0;
    float vfov = 40;
    return camera(lookfrom, lookat, vec3(0, 1, 0), vfov, float(nx) / float(ny), aperture, dist_to_focus);
}

camera normal_camera(int nx, int ny)
{
    vec3 lookfrom(0, 2, 5);
    vec3 lookat(0, 0, -2);
    float dist_to_focus = 10;
    float aperture = 0;

    return camera(lookfrom, lookat, vec3(0, 1, 0), 50, float(nx) / float(ny), aperture, dist_to_focus);
}

#define PBSTR "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
#define PBWIDTH 60

void printProgress(double percentage)
{
    int val = (int)(percentage * 100);
    int lpad = (int)(percentage * PBWIDTH);
    int rpad = PBWIDTH - lpad;
    printf("\r%3d%% [%.*s%*s]", val, lpad, PBSTR, rpad, "");
    fflush(stdout);
}

/* 
240p: 240*320
360p: 360*480
480p: 480*640
720p: 720*1280
1080p: 1080*1920
4k: 2160*3860
*/

std::random_device rd;
std::default_random_engine e{rd()};
std::uniform_real_distribution<float> rand_gen(0, 1);

int main(int argc, char **argv[])
{
    std::clock_t start_t, end_t;

    int nx = 640;
    int ny = 480;
    int ns = 40;
    char name[1000];
    sprintf(name, "cornellbox_%dp_%d.ppm", ny, ns);
    std::ofstream out(name);

    out << "P3\n"
        << nx << " " << ny << "\n255\n";
    float R = cos(M_PI / 4);

    hitable *world = cornell_box();
    // hitable *world = sun_scene();

    camera cam = cornel_camera(nx, ny);
    // camera cam = normal_camera(nx, ny);

    const float gamma = 1.0f / 2.f;

    start_t = std::clock();
    std::cout << "Renderig starts, please wait" << std::endl;
    for (int j = ny - 1; j >= 0; j--)
    {
        for (int i = 0; i < nx; i++)
        {
            vec3 col(0, 0, 0);
            for (int s = 0; s < ns; s++)
            {
                float u = float(i + rand_gen(e)) / float(nx);
                float v = float(j + rand_gen(e)) / float(ny);
                ray r = cam.get_ray(u, v);
                vec3 p = r.point_at_parameter(2.0);
                col += light_source_color(r, world, 0);
                // col += color(r, world, 0);
            }
            col /= float(ns);
            col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
            // col = vec3(pow(col[0], gamma), pow(col[1], gamma), pow(col[2], gamma));

            int ir = int(255.99 * col[0]);
            int ig = int(255.99 * col[1]);
            int ib = int(255.99 * col[2]);
            // if (ir > 255)
            //     std::cerr << i << ' ' << j << " " << ir << std::endl;
            // if (ig > 255)
            //     std::cerr << i << ' ' << j << " " << ig << std::endl;
            // if (ib > 255)
            //     std::cerr << i << ' ' << j << " " << ib << std::endl;

            ir = clamp(ir);
            ig = clamp(ig);
            ib = clamp(ib);

            out << ir << " " << ig << " " << ib << "\n";
        }
        printProgress((ny - j) * 1.0 / ny);
        // if (j % 10 == 0)
        //     std::cout << j << std::endl;
    }
    std::cout << std::endl;
    end_t = std::clock();
    std::cout << "Rendering cost " << (end_t - start_t) / CLOCKS_PER_SEC << "s" << std::endl;
    std::cout << cntPlane << "\t" << cntSphere << '\t' << cntCy << '\t' << cntRect << '\t' << cntObj << std::endl;
    out.close();
    return 0;
}
